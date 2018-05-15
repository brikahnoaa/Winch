/*
 * wispr_flac.c - wispr flac data file I/O utilities
 *
 * FLAC Data file utilities to create, initialize, write, and delete a flac data encoder.
 * These function are wrappers around the libFLAC call, adding functionality for data logging.
 * The basic sequency for calls to encode data is:
 *   - wispr_flac_create(), 
 *   - wispr_flac_set_*(), 
 *   - wispr_flac_init_file(), 
 *   - wispr_flac_write(), 
 *   - wispr_flac_finish(), 
 *   - wispr_flac_delete().
 *
 * The following description of how to use the flac encoder is taken from 
 * the header file stream_encoder.h in the FLAC lib.
 * 
 * The basic usage of this encoder is as follows:
 * - The program creates an instance of an encoder using
 *   FLAC__stream_encoder_new().
 * - The program overrides the default settings using
 *   FLAC__stream_encoder_set_*() functions.  At a minimum, the following
 *   functions should be called:
 *   - FLAC__stream_encoder_set_channels()
 *   - FLAC__stream_encoder_set_bits_per_sample()
 *   - FLAC__stream_encoder_set_sample_rate()
 *   - FLAC__stream_encoder_set_ogg_serial_number() (if encoding to Ogg FLAC)
 *   - FLAC__stream_encoder_set_total_samples_estimate() (if known)
 * - If the application wants to control the compression level or set its own
 *   metadata, then the following should also be called:
 *   - FLAC__stream_encoder_set_compression_level()
 *   - FLAC__stream_encoder_set_verify()
 *   - FLAC__stream_encoder_set_metadata()
 * - The rest of the set functions should only be called if the client needs
 *   exact control over how the audio is compressed; thorough understanding
 *   of the FLAC format is necessary to achieve good results.
 * - The program initializes the instance to validate the settings and
 *   prepare for encoding using
 *   - FLAC__stream_encoder_init_stream() or FLAC__stream_encoder_init_FILE()
 *     or FLAC__stream_encoder_init_file() for native FLAC
 *   - FLAC__stream_encoder_init_ogg_stream() or FLAC__stream_encoder_init_ogg_FILE()
 *     or FLAC__stream_encoder_init_ogg_file() for Ogg FLAC
 * - The program calls FLAC__stream_encoder_process() or
 *   FLAC__stream_encoder_process_interleaved() to encode data, which
 *   subsequently calls the callbacks when there is encoder data ready
 *   to be written.
 * - The program finishes the encoding with FLAC__stream_encoder_finish(),
 *   which causes the encoder to encode any data still in its input pipe,
 *   update the metadata with the final encoding statistics if output
 *   seeking is possible, and finally reset the encoder to the
 *   uninitialized state.
 * - The instance may be used again or deleted with
 *   FLAC__stream_encoder_delete().
 *
 *
 * For each flac data file a separate ascii header file can be created using flac_create_header_file().
 * The header file contains the description for the raw data. 
 *
 * If FLAC_USE_META_DATA is set to non zero, then flac will use metadata blocks in the data files
 * NOTE:  USE OF FLAC METADATA HAS NOT BEEN TESTED.
 *
 * File names contain a user defined prefix and the date:
 * Raw data file names have a .flac extension (prefix_YYMMDD_HHMMSS.flac)
 * Header file names have a .txt extension (prefix_YYMMDD_HHMMSS.txt)
 *
 * THIS SOFTWARE IS PROVIDED BY EOS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL EOS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Embedded Ocean Systems (EOS), 2014
 */
 
#include <stdio.h>
#include <getopt.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/statfs.h>

#include <cycle_count.h>
#include <time.h>

#include "wispr_flac.h"
#include "log.h"

// if set to non zero, then flac will use meta data block in the data files
// DON'T USE METADATA until finding a way to deallocate the memory used when 
// creating Comment_Entrys, otherwise there is a memory leak
#define WISPR_FLAC_USE_META_DATA  0

extern int verbose_level;

//
// make filename from time
//
void make_filename(char *name, char *prefix, time_t sec)
{
	// make new file name from the prefix and date
	char str[32];
	struct tm time;
	if(sec <= 0) {  // use gettimeof day
		struct timeval now;
		gettimeofday(&now, NULL);
		gmtime_r(&now.tv_sec, &time);
	} else {
		time_t s = (time_t)sec;
		gmtime_r((time_t *)&s, &time);
	}
	strftime(str, sizeof(str), "%g%m%d_%H%M%S", &time); 
	sprintf(name, "%s%s", prefix, str);
}

/*
 *  Wrapper for FLAC__stream_encoder_new
 */
int wispr_flac_create(wispr_flac_t *file, char *path, char *prefix)
{ 
	// initial/default values
	memset(file, 0, sizeof(wispr_flac_t));
	
	// default values
	//file->bfree_min = 8000; // 8000*4096 = 32 Mbytes free
	file->info.compression = 4;
	file->info.nbps = 16;
	file->info.fs = 0;
	file->info.nchans = 1;
	file->info.nsamps = 0;
	if(path == NULL) {
		strcpy(file->path, "/mnt");
	} else {
		strncpy(file->path, path, 32);
	}
	if(prefix == NULL) {
		strcpy(file->prefix, "wispr_");
	} else {
		strncpy(file->prefix, prefix, 16);
	}

	// meta data and disk space flags
	file->use_metadata = 0; // default is no metadata
	file->check_min_bfree = 0;  // default is to not statfs for each file

	// create new encoder
	if((file->encoder = FLAC__stream_encoder_new()) == NULL) {
		log_printf( "wispr_flac_create: Error allocating flac encoder\n");
		return(-1);
	}

	// Flac meta data 
	file->metadata[0] = NULL;
	file->metadata[1] = NULL;
	
	// -- need to verify that this doesn't cause memory leaks
	if(file->use_metadata > 0) {
		
		// allocate new meta data and padding block
		if((file->metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT)) == NULL) {
			log_printf( "wispr_flac_create: Error allocating flac meta data comment\n");
			return(-1);
		}		
		// is padding necessary ???
		if((file->metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING)) == NULL) {
			log_printf( "wispr_flac_create: Error allocating flac meta data padding\n");
			return(-1);
		}
		/* set the padding length */
		file->metadata[1]->length = 1024; 
	}

	file->encoder_state = FLAC__STREAM_ENCODER_UNINITIALIZED;

	return(1);
}

/*
 *  Wrapper for FLAC__metadata_object_delete
 */
void wispr_flac_delete(wispr_flac_t *file)
{
	FLAC__bool ok = true;
	if(file->use_metadata > 0)  {
		if(file->metadata[0] != NULL) FLAC__metadata_object_delete(file->metadata[0]);
		if(file->metadata[1] != NULL) FLAC__metadata_object_delete(file->metadata[1]);
		file->metadata[0] = NULL;
		file->metadata[1] = NULL;
	}
	if(file->encoder != NULL) {
		ok = FLAC__stream_encoder_finish(file->encoder);
		if(ok) FLAC__stream_encoder_delete(file->encoder);
		file->encoder = NULL;
	}

}

/*
 *  Set file attributes
 */
int wispr_flac_set_compression_level(wispr_flac_t *file, int comp_level) 
{
	if(comp_level < 1 || comp_level > 8) comp_level = 4;
	file->info.compression = comp_level;
    return(comp_level);
}

// only handles 16 or 24 bits data 
int wispr_flac_set_bits_per_sample(wispr_flac_t *file, int nbps) 
{
    if((nbps == 8) || (nbps == 16) || (nbps == 24))  file->info.nbps = nbps;
	else file->info.nbps = 16;
    return(file->info.nbps);
}

int wispr_flac_set_channels(wispr_flac_t *file, int nchans) 
{
	file->info.nchans = nchans;
    return(nchans);
}

int wispr_flac_set_sample_rate(wispr_flac_t *file, int fs) 
{
	file->info.fs = fs;
    return(fs);
}

int wispr_flac_set_total_samples_estimate(wispr_flac_t *file, int nsamps) 
{
	file->info.nsamps = nsamps;
    return(nsamps);
}

int wispr_flac_set_total_samples(wispr_flac_t *file, int nsamps) 
{
	file->info.nsamps = nsamps;
    return(nsamps);
}

int wispr_flac_set_min_free_blocks(wispr_flac_t *file, int bfree_min) 
{
    if(bfree_min < 0) {
		bfree_min = 0;	
	}
	file->check_min_bfree = 1;
	file->bfree_min = bfree_min;
    return(bfree_min);
}

int wispr_flac_use_metadata(wispr_flac_t *file) 
{
	file->use_metadata = 1;
    return(1);
}

int wispr_flac_set_description(wispr_flac_t *file, char *str) 
{
	if(str == NULL) return(0);
	strncpy(file->description, str, sizeof(file->description));
    return(1);
}

/*
 *  Wrapper to initialize a new file using an existing encoder.
 *  Calls :
 *   - FLAC__stream_encoder_finish if encoder has been initilized already
 *   - FLAC__stream_encoder_set_*
 *   - FLAC__stream_encoder_init_file
 *   - FLAC__stream_encoder_set_metadata if metadata is used
 */  
FLAC__bool wispr_flac_init_file(wispr_flac_t *file, u_int32_t sec, u_int32_t usec)
{
	FLAC__bool ok = true;
	FLAC__StreamEncoderInitStatus init_status;
    struct statfs stat;

	if(file->encoder == NULL) {
		log_printf( "wispr_flac_init_file: invalid encoder\n");
		return(false);
	}

	// If there is an existing open encoder, finish it.
	if(file->encoder_state == FLAC__STREAM_ENCODER_OK) {
	  ok = FLAC__stream_encoder_finish(file->encoder);
	  if(!ok) {
		log_printf( "wispr_flac_init_file ERROR: flac encoder finish failed.\n");
	  } 
	}

    // set file open time
	struct tm time;
	if(sec <= 0) {  // use gettimeofday
		struct timeval now;
		gettimeofday(&now, NULL);
		gmtime_r(&now.tv_sec, &time);
		// set file open time with now
		file->info.sec = now.tv_sec;
		file->info.usec = now.tv_usec;
	} else {
	    time_t s = (time_t)sec;
	    gmtime_r((time_t *)&s, &time);
		// set file open time with args
		file->info.sec = sec;
		file->info.usec = usec;
	}

	// make the file name with the open time
	char str[32];
    strftime(str, sizeof(str), "%g%m%d_%H%M%S", &time); 
    sprintf(file->name, "%s%s", file->prefix, str);

	// set file info
	int comp_level = file->info.compression;
	int nbps = file->info.nbps;
	int fs = file->info.fs;  // sampling rate 
	int nchans = file->info.nchans;
	u_int32_t nsamps = file->info.nsamps;

    // check disk space when file is opened
    // warning - statfs could be slow and cause buffer over runs when a new file is created.
	if(file->check_min_bfree) {
		if((statfs(file->path, &stat)) < 0 ) {
			log_printf( "wispr_flac_init_file: Failed to stat %s:\n", file->path);
			file->bfree = 0;
			file->blocks = 0;
		} else {
			file->bfree = (unsigned long)(stat.f_bavail);
			file->blocks = (unsigned long)(stat.f_blocks);
		}
	}

	// Set encoder settings, which are reset by _finish
	if(verbose_level > 2) log_printf("wispr_flac_init_file: comp_level %d, fs %d, bps %d, total nsamps %u\n",
		comp_level, fs, nbps, nsamps);

	//ok &= FLAC__stream_encoder_set_verify(encoder, ok);
	ok &= FLAC__stream_encoder_set_compression_level(file->encoder, comp_level);
	ok &= FLAC__stream_encoder_set_channels(file->encoder, nchans); 
	ok &= FLAC__stream_encoder_set_bits_per_sample(file->encoder, nbps);
	ok &= FLAC__stream_encoder_set_sample_rate(file->encoder, fs);
	ok &= FLAC__stream_encoder_set_total_samples_estimate(file->encoder, (FLAC__uint64)nsamps);

	if(!ok) {
		log_printf( "wispr_flac_init_file ERROR: flac encoder set failed.\n");
		return(ok);
	}

	// Flac meta data 
	// -- need to verify that there are no memory leaks here
	// -- so use metadata with caution
	if(file->use_metadata > 0) {

		FLAC__StreamMetadata_VorbisComment_Entry entry;

		// metadata date and time string
		char mstr[32];
        char dtstr[32];
		gmtime_r((time_t *)&(file->info.sec), &time);
        //strftime(str, sizeof(str), "%g%m%d_%H%M%S", tm);
        strftime(mstr, sizeof(mstr), "%m/%d/%y %H:%M:%S", &time);  // m/d/y H:M:S time for metadata
        //fprintf (dbg_fp, "%s.%05ld\n", mstr, adc->time.tv_nsec/1000000L);
        sprintf(dtstr, "%s.%03ld", mstr, file->info.usec/1000L);

		/* set meta data */
		/* there are many tag (vorbiscomment) functions but these are convenient for this particular use: */
		/* copy=false: let metadata object take control of entry's allocated string */
		if(
			/* there are many tag (vorbiscomment) functions but these are convenient for this particular use: */
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "DATE", dtstr) ||
			!FLAC__metadata_object_vorbiscomment_append_comment(file->metadata[0], entry, false) ||
			!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "DESCRIPTION", file->description) ||
			!FLAC__metadata_object_vorbiscomment_append_comment(file->metadata[0], entry, /*copy=*/false)
			) 
		{
			log_printf( "wispr_flac_init_file ERROR: out of memory or tag error\n");
			ok = false;
		}

		ok = FLAC__stream_encoder_set_metadata(file->encoder, file->metadata, 2);
		if(!ok) {
			log_printf( "wispr_flac_init_file ERROR: set_metadata failed\n");
			return(ok);
		}
		if(verbose_level > 2) log_printf( "wispr_flac_init_file: metadata set ok\n");

	}

	/* initialize encoder for first buffer of new file*/
	char filename[64];
	sprintf(filename, "%s/%s.flac", file->path, file->name);
	init_status = FLAC__stream_encoder_init_file(file->encoder, filename, NULL, NULL);
	
	if(init_status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
		log_printf( "wispr_flac_init_file ERROR: initializing encoder: %s\n", 
			FLAC__StreamEncoderInitStatusString[init_status]);
		//log_printf("Encoder initialize fail, new file %s\n", filename);
		return(false);
	}
	
	file->encoder_state = FLAC__STREAM_ENCODER_OK;

	if(file->check_min_bfree) {
		log_printf("New file %s, %0.2f%% fs used\n",
			file->name, 100.0*(float)(file->blocks - file->bfree)/(float)(file->blocks));	
	} else {
		log_printf("New file %s\n", file->name);			
	}

    return(ok);
}

/*
 *  Finish encoder
 */
void wispr_flac_finish(wispr_flac_t *file)
{
	FLAC__bool ok = true;

	if(file->encoder == NULL) {
		log_printf( "wispr_flac_finish: invalid encoder\n");
		return;
	}

	if(file->encoder_state == FLAC__STREAM_ENCODER_UNINITIALIZED) {
		if(verbose_level > 1) 
			log_printf( "wispr_flac_finish: encoder is not initialized\n");
		return;
	}

	if(file->encoder_state == FLAC__STREAM_ENCODER_OK ) {
		ok = FLAC__stream_encoder_finish(file->encoder);
		if(!ok) {
			log_printf( "wispr_flac_finish ERROR: flac encoder finish failed.\n");
		} else if(verbose_level > 1) 
			log_printf( "wispr_flac_finish: encoder finish\n");
	}

	file->encoder_state = FLAC__STREAM_ENCODER_UNINITIALIZED;

}

/*
 * Write data to file
 * Calls FLAC__stream_encoder_process_interleaved
 * the data is passed in as a char* and then cast into the appropriate 
 * type as specified by the number of bits (nbps).  
 * nbps specifies the type of data (i.e. nbps = 16 id for int16_t data),
 * not necessarily the number of bit for encoding.
 * 24 bit encoding uses 32 bit data, for example.
 */
FLAC__int32 pcm_buffer[WISPR_FLAC_BUF_SIZE];  

int wispr_flac_write(wispr_flac_t *file, char *data, u_int32_t nsamps, int nbps)
{
   if(file == NULL) {
      log_printf( "wispr_flac_write: invalid file objects\n");
      return(0);
   }

   if((nbps != 16) && (nbps != 24) && (nbps != 32)) {
	   log_printf( "wispr_flac_write: does not support %d bit data.\n", nbps);
      return(0);
   }

   if(file->encoder_state == FLAC__STREAM_ENCODER_UNINITIALIZED) {
      log_printf( "wispr_flac_write: file object not open\n");
      return(0);
   }

   // check that there's enough space on file system
   if(file->check_min_bfree) {
      if((file->blocks > 0) && (file->bfree <= file->bfree_min)) {
         if(verbose_level) log_printf("wispr_flac_write: file system full");
         if(verbose_level > 0) {
            log_printf( "wispr_flac_write: only %ld blocks free, no data written.\n", file->bfree);
         }
         return(0); 
      }
   }
   
   // write 16 bit data
   size_t nwrt = 0;
   if(nbps == 16) {
	   int16_t *buf16 = (int16_t *)data;
	   nwrt = wispr_flac_write_int16(file, buf16, nsamps);
   }
   
   // write 24 and 32 bit data
   if((nbps == 24) || (nbps == 32)) {
	   int32_t *buf32 = (int32_t *)data;
	   nwrt = wispr_flac_write_int32(file, buf32, nsamps);
   }

   return(nwrt);
}

int wispr_flac_write_int16(wispr_flac_t *file, int16_t *data, u_int32_t nsamps)
{
   FLAC__bool ok = true;

   // write the data buffer to the flac encoder by
   // breaking the data into buffers of size bufsize and encoding each buffer.
   size_t n, i;
   //int shift = file->info.shift;  
   size_t k = 0;
   int bufsize = WISPR_FLAC_BUF_SIZE;
   int nbufs = nsamps / bufsize;
   // this should work for 16, 24, and 32 bit data
   for( n = 0; n < nbufs; n++ ) {
      // fill the pcm buffer, shifting each word
      for( i = 0; i < bufsize ; i++) {
         pcm_buffer[i] = (FLAC__int32)data[k++];
      }
      // flac stream encode the pcm buffer
      ok = FLAC__stream_encoder_process_interleaved(file->encoder, pcm_buffer, bufsize);
	  if(ok == false) {
		log_printf( "wispr_flac_write_int16: encoder failed.\n");
	  }
   }
   
   // if nsamps is not a multiple of bufsize then there will be leftover samples
   int leftover = nsamps - nbufs * bufsize;
   if(leftover > 0) {
      for ( i = 0; i < leftover; i++ ) {
         pcm_buffer[i] = (FLAC__int32)data[k++];
      }
      // flac stream encode the pcm buffer
      ok = FLAC__stream_encoder_process_interleaved(file->encoder, pcm_buffer, leftover);
	  if(ok == false) {
		log_printf( "wispr_flac_write_int16: encoder failed.\n");
	  }
   }

   //for(i = 0; i < 20; i++) {
   // fprintf(stdout, "- %d (0x%08x)\n", (FLAC__int32)data[i], (FLAC__int32)data[i]);
   //}
   
   if(verbose_level > 1) {
      log_printf( "wispr_flac_write_int16: %ld samples written\n", k);
      //log_printf( "wispr_flac_write_int16: %ld samples written, %.2f flac buffers.\n", k, 
	  //  (float)nbufs + (float)bufsize/(float)bufsize);
   }
   return(k);
}

int wispr_flac_write_int32(wispr_flac_t *file, int32_t *data, u_int32_t nsamps)
{
   FLAC__bool ok = true;

   // write the data buffer to the flac encoder by
   // breaking the data into buffers of size bufsize and encoding each buffer.
   size_t n, i;
   //int shift = file->info.shift;
   size_t k = 0;
   int bufsize = WISPR_FLAC_BUF_SIZE;
   int nbufs = nsamps / bufsize;
   // this should work for 16, 24, and 32 bit data
   for( n = 0; n < nbufs; n++ ) {
      // fill the pcm buffer, shifting each word
      for( i = 0; i < bufsize ; i++ ) {
		 // cast the unsigned data word into a signed word before shifting
		 // otherwise, the sign bit is lost in the shift
         pcm_buffer[i] = (FLAC__int32)data[k++];
      }
      // flac stream encode the pcm buffer
      ok = FLAC__stream_encoder_process_interleaved(file->encoder, pcm_buffer, bufsize);
   }
   
   // if nsamps is not a multiple of bufsize then there will be leftover samples
   int leftover = nsamps - nbufs * bufsize;
   if(leftover > 0) {
      for ( i = 0; i < leftover; i++ ) {
         pcm_buffer[i] = (FLAC__int32)data[k++];
      }
      // flac stream encode the pcm buffer
      ok = FLAC__stream_encoder_process_interleaved(file->encoder, pcm_buffer, leftover);
   }

   if(verbose_level > 1) {
      log_printf( "wispr_flac_write_int32: %ld samples written.\n", k);
   }

   return(k);
}

/*
 * Write header information to a text file
 */
int wispr_flac_create_header_file(wispr_flac_t *file, u_int32_t sec, u_int32_t usec)
{
	char filename[64], str[32];
	int nwrt=0;
	FILE *fp;

	// make new txt file name from the path, prefix and date
	time_t s = (time_t)sec;
	struct tm time;
	gmtime_r((time_t *)&s, &time);
	strftime(str, sizeof(str), "%g%m%d_%H%M%S", &time); 
	sprintf(filename, "%s/%s%s.txt", file->path, file->prefix, str);

	// open current data file for appending
	fp = fopen(filename, "w+");
	if(fp <= 0) {
		log_printf( "wispr_flac_create_header_file: Can't open output file, no data written\n");
		return(-1);
	}

	// write the header file text
	fprintf(fp, "%% WISPR FLAC DATA FILE HEADER\n");
	fprintf(fp, "file name: %s\n", file->name);
	fprintf(fp, "file description: %s\n", file->description);
	fprintf(fp, "compression level: %d\n", file->info.compression);
	fprintf(fp, "start time sec: %u\n", (unsigned int)sec);
	fprintf(fp, "start time usec: %u\n", (unsigned int)usec);
	fprintf(fp, "samples per second: %d\n", file->info.fs);
	fprintf(fp, "total samples: %u\n", (unsigned int)file->info.nsamps);
	fprintf(fp, "bits per sample: %d\n", file->info.nbps);
	fprintf(fp, "channels: %d\n", file->info.nchans);

	fclose(fp);

	return(nwrt);
}

