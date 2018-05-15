/*
 * dat_file.c - wispr data file I/O utilities
 *
 * Data is writen to file as raw binary.
 * Data words are either 16, 24, or 32 bits as specified by dat_set_bits_per_sample().
 * If the number of bits per sample (nbps) is 16, then the data word is shifted (>>) before written to file.
 * The bitshift is set using dat_set_bits_per_sample().
 * 
 * There is NO header on the data file.
 * For each data file there's a separate ascii header file with the same base name.
 * The header file contains the description for the raw data. 
 * 
 * File names contain a user defined prefix and the date:
 * Raw data file names have a .flac extension (prefix_YYMMDD_HHMMSS.flac)
 * Header file names have a .txt extension (prefix_YYMMDD_HHMMSS.txt)
 *
 * ------
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

#include "wispr.h"
#include "dat_file.h"
#include "log.h"

extern int verbose_level;

/*
 *  Create a dat file object with default settings.
 *  The actual file is created using fopen when dat_init_new() is called.  
 */
int dat_create(dat_file_t *file, char *path, char *prefix)
{ 
	struct statfs stat;

	// initial/default values
	memset(file, 0, sizeof(dat_file_t));
	
	// default values
	file->bfree_min = 8000; // 8000*4096 = 32 Mbytes free
	file->log[0] = 0;
	file->info.compression = 0;
	file->info.nbps = 16;
	file->info.fs = 0;
	file->info.nchans = 1;
	file->info.nsamps = 0;
	file->info.gain = 0;
	file->info.shift = 0;
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

	// check disk space
	if((statfs(path, &stat)) < 0 ) {
		log_printf( "dat_create: Failed to stat %s\n", path);
		return(-1);
	}
	
	file->bsize = (unsigned long)(stat.f_bsize);  // fs block size in bytes
	//file->bsize = FILE_BLOCK_SIZE;  // 
	file->bfree = (unsigned long)(stat.f_bavail);
	file->blocks = (unsigned long)(stat.f_blocks);

	return(1);
}

void dat_close(dat_file_t *file)
{
	// nothing to do
}

int dat_set_bits_per_sample(dat_file_t *file, int nbps) 
{
    if((nbps != 8) & (nbps != 16) & (nbps != 24) & (nbps != 32)) nbps = 16;
	file->info.nbps = nbps;
    return(nbps);
}

int dat_set_channels(dat_file_t *file, int nchans) 
{
    if(nchans > 1) nchans = 1;
	file->info.nchans = nchans;
    return(nchans);
}

int dat_set_sample_rate(dat_file_t *file, int fs) 
{
	file->info.fs = fs;
    return(fs);
}

int dat_set_total_samples_estimate(dat_file_t *file, int nsamps) 
{
	file->info.nsamps = nsamps;
    return(nsamps);
}

int dat_set_total_samples(dat_file_t *file, int nsamps) 
{
	file->info.nsamps = nsamps;
    return(nsamps);
}

int dat_set_gain(dat_file_t *file, int gain) 
{
    if(gain > 3) gain = 3;
    if(gain < 0) gain = 0;
	file->info.gain = gain;
    return(gain);
}

int dat_set_bitshift(dat_file_t *file, int shift) 
{
    if(shift > 8) shift = 8;
    if(shift < 0) shift = 0;
	file->info.shift = shift;
    return(shift);
}

int dat_set_log_filename(dat_file_t *file, char *name) 
{
	strncpy(file->log, name, sizeof(file->log));
    return(1);
}


// 
// Initialize a new file using the time as the file name.
//
int dat_init_new(dat_file_t *file, u_int32_t sec)
{
	char str[64];
    struct statfs stat;

	if(file == NULL) {
		log_printf( "dat_init_new: invalid file objects\n");
		return(0);
	}

    // make new file name from the prefix and date
	time_t s = (time_t)sec;
	struct tm time;
	gmtime_r((time_t *)&s, &time);
    strftime(str, sizeof(str), "%g%m%d_%H%M%S", &time); 
    sprintf(file->name, "%s%s", file->prefix, str);
      
	// set file info
	file->info.sec = sec;
	file->info.usec = 0;

    // check disk space
    // - statfs could be slow and cause buffer over runs when a new file is created.
	if((statfs(file->path, &stat)) < 0 ) {
		log_printf( "dat_init_new: Failed to stat %s\n", file->path);
		file->bfree = 0;
		file->blocks = 0;
	} else {
		file->bfree = (unsigned long)(stat.f_bavail);
		file->blocks = (unsigned long)(stat.f_blocks);
	}

   // create data file for appending
   char filename[64];
   sprintf(filename, "%s/%s.dat", file->path, file->name);
   FILE *fp = fopen(filename, "w");
   if(fp == NULL) {
      log_printf( "dat_init_new: Can't create output file, no data written\n");
      return(-1);
   }
   fclose(fp);

//   snprintf(str, sizeof(str), "New file %s.dat, %0.2f percent of fs used.\n",
//		file->name, 100.0*(float)(file->blocks - file->bfree)/(float)(file->blocks));
//	log_printf(str);

   log_printf("New file %s.dat, %0.2f percent of fs used.\n",
		file->name, 100.0*(float)(file->blocks - file->bfree)/(float)(file->blocks));

   return(1);
}

/*
 * Write data buffer to file using the specified number of samples and bytes per sample.
 * Opens and closes the file on each write call.
 */
int dat_write(dat_file_t *file, char *data, u_int32_t nsamps, int nbytes_per_samp)
{
   char filename[64];

   if((file->blocks > 0) && (file->bfree <= file->bfree_min)) {
      if(verbose_level) log_printf("dat_write: file system full");
      if(verbose_level > 0) {
         log_printf( "dat_write: only %ld blocks free, no data written.\n", 
            file->bfree);
      }
      return(0); 
   }

   // open current data file for appending
   sprintf(filename, "%s/%s.dat", file->path, file->name);
   FILE *fp = fopen(filename, "a+");
   if(fp == NULL) {
      log_printf( "dat_write: Can't open output file, no data written\n");
      return(-1);
   }

   // write data buffer
   size_t nwrt = fwrite(data, nbytes_per_samp, nsamps, fp);
   if(nwrt < nsamps) {
      log_printf( "dat_write: error writing data to %s\n", file->name);
   }

   if(verbose_level > 1) {
      log_printf( "dat_write: %ld samples written, %ld blks free\n",
         nwrt, (file->bfree));
   }

   // force output to file
   fflush(fp);
   fclose(fp);

   return(nwrt);
}

/*
 * Write uint32 data buffer to file using the specified number of bits per sample (file.nbps).
 * If nbps == 16, then bit shifting is applied using the value in file.shift.
 */
int dat_write_int32(dat_file_t *file, u_int32_t *data, u_int32_t nsamps)
{
   int nbps = file->info.nbps;
   int shift = file->info.shift;

   // write the data buffer
   size_t nbytes = 4;  // 4 byte word by default

   // overwrite 32 bit data words in buffer as 16 bit words after bit shifting
   if(nbps == 16) {
	   size_t n = 0;
	   size_t i = 0;
	   u_int16_t *buf16 = (u_int16_t *)data;
	   for(n = 0; n < nsamps; n++) {
		   buf16[i++] = (u_int16_t)(data[n] >> shift);
	   }
	   nbytes = 2;   
   }

   // overwrite 32 bit words in buffer as 24 bit words 
   if(nbps == 24) {
	   size_t i, j, n;
	   unsigned char *buf32;
	   unsigned char *buf24;
	   buf32 = (unsigned char *)data;
	   buf24 = (unsigned char *)data;
	   for(n = 0; n < nsamps; n++) {
		   i = n*4;
		   j = n*3;
		   buf24[j] = buf32[i+1]; 
		   buf24[j+1] = buf32[i+2]; 
		   buf24[j+2] = buf32[i+3]; 
	   }
	   nbytes = 3;   
   }

   // write the scaled buffer to file
   int nwrt = dat_write(file, (char *)data, nsamps, nbytes);

   return(nwrt);
}

/*
 * write header information to a text file
 */
int dat_write_header(dat_file_t *file, u_int32_t sec, u_int32_t usec)
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
		log_printf( "dat_write_header: Can't open output file, no data written\n");
		return(-1);
	}

	// write the header file text
	fprintf(fp, "%% WISPR DATA FILE HEADER\n");
	fprintf(fp, "file name: %s\n", file->name);
	fprintf(fp, "compression level: %d\n", file->info.compression);
	fprintf(fp, "start time sec: %u\n", (unsigned int)sec);
	fprintf(fp, "start time usec: %u\n", (unsigned int)usec);
	fprintf(fp, "samples per second: %d\n", file->info.fs);
	fprintf(fp, "total samples: %u\n", (unsigned int)file->info.nsamps);
	fprintf(fp, "bits per sample: %d\n", file->info.nbps);
	fprintf(fp, "channels: %d\n", file->info.nchans);
	fprintf(fp, "gain: %d\n", file->info.gain);
	fprintf(fp, "shift: %d\n", file->info.shift);

	fclose(fp);

	return(nwrt);
}


