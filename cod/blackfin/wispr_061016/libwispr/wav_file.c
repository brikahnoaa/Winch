/*
 * wav_file.c - wave data file I/O utilities
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
#include "wav_file.h"
#include "log.h"

extern int verbose_level;

/*
* The canonical WAVE format starts with the RIFF header:
* Offset  Size Name           Description
*------------------------------------------------------------------------------
* 0       4    ChunkID        Contains the letters "RIFF" in ASCII form
* 4       4    ChunkSize      4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
* 8       4    Format          Contains the letters "WAVE"
* 12      4    Subchunk1ID    Contains the letters "fmt "
* 16      4    Subchunk1Size  16 for PCM.  This is the size of the Subchunk that follows this number.
* 20      2    AudioFormat    PCM = 1 (i.e. Linear quantization)
* 22      2    NumChannels    Mono = 1, Stereo = 2, etc.
* 24      4    SampleRate     Hz
* 28      4    ByteRate       == SampleRate * NumChannels * BitsPerSample/8
* 32      2    BlockAlign     == NumChannels * BitsPerSample/8
* 34      2    BitsPerSample  8 bits = 8, 16 bits = 16, etc.
* 36      4    Subchunk2ID    Contains the letters "data"
* 40      4    Subchunk2Size  == NumSamples * NumChannels * BitsPerSample/8
* 44      *    Data           The actual sound data.
*/

size_t wav_read_header(FILE *fp, wav_file_header_t *wav)
{
  size_t nrd = 0;
  nrd = fread(wav->ChunkID,1,4,fp);
  nrd += fread(&(wav->ChunkSize),1,4,fp);
  nrd += fread(wav->Format,1,4,fp);
  nrd += fread(wav->Subchunk1ID,1,4,fp);
  nrd += fread(&wav->Subchunk1Size,1,4,fp);
  nrd += fread(&wav->AudioFormat,1,2,fp);
  nrd += fread(&wav->NumChannels,1,2,fp);
  nrd += fread(&wav->SampleRate,1,4,fp);
  nrd += fread(&wav->ByteRate,1,4,fp);
  nrd += fread(&wav->BlockAlign,1,2,fp);
  nrd += fread(&wav->BitsPerSample,1,2,fp);
  nrd += fread(wav->Subchunk2ID,1,4,fp);
  nrd += fread(&wav->Subchunk2Size,1,4,fp);
  //nrd = fread(wav, 1, 44, fp);
  return(nrd);
}

size_t wav_write_header(FILE *fp, wav_file_header_t *wav)
{
  size_t nwrt = 0;
  nwrt = fwrite(wav->ChunkID,1,4,fp);
  nwrt += fwrite(&wav->ChunkSize,1,4,fp);
  nwrt += fwrite(wav->Format,1,4,fp);
  nwrt += fwrite(wav->Subchunk1ID,1,4,fp);
  nwrt += fwrite(&wav->Subchunk1Size,1,4,fp);
  nwrt += fwrite(&wav->AudioFormat,1,2,fp);
  nwrt += fwrite(&wav->NumChannels,1,2,fp);
  nwrt += fwrite(&wav->SampleRate,1,4,fp);
  nwrt += fwrite(&wav->ByteRate,1,4,fp);
  nwrt += fwrite(&wav->BlockAlign,1,2,fp);
  nwrt += fwrite(&wav->BitsPerSample,1,2,fp);
  nwrt += fwrite(wav->Subchunk2ID,1,4,fp);
  nwrt += fwrite(&wav->Subchunk2Size,1,4,fp); 
  //nwrt = fwrite(wav, 1, 44, fp);
  return(nwrt);
}

void wav_print_header(FILE *fp, wav_file_header_t *wave)
{
  char str[5];
  strncpy(str, wave->ChunkID,4); str[4]=0;
  fprintf(stdout, "ChunkID:        %s\n", str); 
  fprintf(stdout, "ChunkSize:      %d\n", wave->ChunkSize);
  strncpy(str, wave->Format,4); str[4]=0;
  fprintf(stdout, "Format:         %s\n", str);
  strncpy(str, wave->Subchunk1ID,4); str[4]=0;
  fprintf(stdout, "Subchunk1ID:    %s\n", str);
  fprintf(stdout, "Subchunk1Size:  %d\n", wave->Subchunk1Size);
  fprintf(stdout, "AudioFormat:    %d\n", wave->AudioFormat);
  fprintf(stdout, "NumChannels:    %d\n", wave->NumChannels);
  fprintf(stdout, "SampleRate:     %d\n", wave->SampleRate);
  fprintf(stdout, "ByteRate:       %d\n", wave->ByteRate);
  fprintf(stdout, "BlockAlign:     %d\n", wave->BlockAlign);
  fprintf(stdout, "BitsPerSample:  %d\n", wave->BitsPerSample);
  strncpy(str, wave->Subchunk2ID,4); str[4]=0;
  fprintf(stdout, "Subchunk2ID:    %s\n", str);
  fprintf(stdout, "Subchunk2Size:  %d\n", wave->Subchunk2Size);
}

// initialize wav header with default values
//
static void wav_init_header(wav_file_header_t *wav, int nbps, int fs, int nchans)
{
  wav->ChunkID[0] = 'R';
  wav->ChunkID[1] = 'I';
  wav->ChunkID[2] = 'F';
  wav->ChunkID[3] = 'F';
  wav->ChunkSize = 36; // = 4 + 8 + 16 + 8 + 0;
  wav->Format[0] = 'W';
  wav->Format[1] = 'A'; 
  wav->Format[2] = 'V';
  wav->Format[3] = 'E'; 
  wav->Subchunk1ID[0] = 'f';
  wav->Subchunk1ID[1] = 'm';
  wav->Subchunk1ID[2] = 't';
  wav->Subchunk1ID[3] = ' ';  // space, not null
  wav->Subchunk1Size = 16;
  wav->AudioFormat = 1;
  wav->NumChannels = 1;
  wav->SampleRate = fs;
  wav->ByteRate = fs * nchans * nbps / 8;
  wav->BlockAlign = nchans * nbps / 8;
  wav->BitsPerSample = nbps;
  wav->Subchunk2ID[0] = 'd';
  wav->Subchunk2ID[1] = 'a';
  wav->Subchunk2ID[2] = 't';
  wav->Subchunk2ID[3] = 'a';
  wav->Subchunk2Size = 0;  // no data yet
}

/*
 *  Create a wav file object with default settings.
 *  The actual file is created when wav_open() is called.  
 */
int wav_create(wav_file_t *file, char *path, char *prefix)
{ 
	// initial/default values
	memset(file, 0, sizeof(wav_file_t));
	
	// default values
	file->nbps = 16;
	file->nchans = 1;

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

	return(1);
}

void wav_close(wav_file_t *file)
{
	// nothing to do
}

int wav_set_bits_per_sample(wav_file_t *file, int nbps) 
{
    if((nbps == 8)||(nbps == 16)||(nbps == 24)||(nbps == 32))
		file->nbps = nbps;
	else 
		file->nbps = 16;
    return(file->nbps);
}

int wav_set_channels(wav_file_t *file, int nchans) 
{
    if(nchans > 1) nchans = 1;
	file->nchans = nchans;
    return(file->nchans);
}

int wav_set_sample_rate(wav_file_t *file, int fs) 
{
	file->fs = fs;
    return(fs);
}

int wav_set_total_samples_estimate(wav_file_t *file, int nsamps) 
{
	file->nsamps = nsamps;
    return(nsamps);
}

int wav_set_total_samples(wav_file_t *file, int nsamps) 
{
	file->nsamps = nsamps;
    return(nsamps);
}

int wav_set_description(wav_file_t *file, char *str) 
{
	if(str == NULL) return(0);
	strncpy(str, file->description, sizeof(file->description));
    return(1);
}

// 
// Open and initialize a new wav file using the time as the file name.
//
int wav_open(wav_file_t *file, u_int32_t sec)
{
	char str[64];

	if(file == NULL) {
		log_printf( "wav_open: invalid file objects\n");
		return(0);
	}

    // set file open time
	if( sec <= 0 ) {  // use gettimeofday
		struct tm time;
		struct timeval now;
		gettimeofday(&now, NULL);
		gmtime_r(&now.tv_sec, &time);
		// set file open time with now
		sec = now.tv_sec;
	}
	
    // make new file name from the prefix and date
	time_t s = (time_t)sec;
	struct tm time;
	gmtime_r((time_t *)&s, &time);
    strftime(str, sizeof(str), "%g%m%d_%H%M%S", &time); 
    sprintf(file->name, "%s%s", file->prefix, str);
    
	// set file info
	file->sec = sec;
	file->usec = 0;

   // Create an empty file for output operations. 
   // If a file with the same name already exists, its contents are discarded 
   // and the file is treated as a new empty file.
   char filename[64];
   sprintf(filename, "%s/%s.wav", file->path, file->name);
   FILE *fp = fopen(filename, "w");
   if(fp == NULL) {
      log_printf( "wav_open: Can't create output file, no data written\n");
      return(-1);
   }
   
   // initialize the wav header
   wav_init_header(&file->hdr, file->nbps, file->fs, file->nchans);
	
   // write wav header
   if(wav_write_header(fp, &file->hdr) <= 0) {
      log_printf( "wav_write: error writing wav header\n");
   }
   
   fclose(fp);

   log_printf("New file %s.wav\n", file->name);

   return(1);
}

/*
 * Write data buffer to file using the specified number of samples and bytes per sample.
 * Opens and closes the file on each write call.
 */
size_t wav_write(wav_file_t *file, char *data, u_int32_t nsamps)
{
   char filename[64];

   // Open file for update (both for input and output). The file must exist
   sprintf(filename, "%s/%s.wav", file->path, file->name);
   FILE *fp = fopen(filename, "r+");
   if(fp == NULL) {
      log_printf( "wav_write: Can't open output file, no data written\n");
      return(-1);
   }

   // read wav header
   //fseek(fp, 0, SEEK_SET);  
   //size_t nrd = fread(&file->hdr, 1, 44, fp);
   size_t nrd = wav_read_header(fp, &file->hdr);
   if(nrd < 44) {
      log_printf( "wav_write: error reading wav header\n");
   }

   // check nbps
   int nbps = file->nbps;
   int nchans = file->nchans;
   if(nbps != file->hdr.BitsPerSample) {
      log_printf( "wav_write: inconsistent data format\n");
      return(-1);
   }
   
   // update the header:
   //  - Subchunk2Size - add number of bytes in this buffer
   //  - ChunkSize - for PCM, 36 + Subchunk2Size.
   file->hdr.Subchunk2Size += (int)(nsamps * nchans * nbps/8); // add number of bytes 
   file->hdr.ChunkSize = 36 + file->hdr.Subchunk2Size;
   
   // rewrite the header
   fseek(fp, 0, SEEK_SET);  
   //size_t nwrt = fwrite(&file->hdr, 1, 44, fp);
   size_t nwrt = wav_write_header(fp, &file->hdr);
   if(nwrt < 44) {
      log_printf( "wav_write: error writing wav header\n");
   }
   //wav_print_header(fp, &file->hdr);
   
   // seek to end of file to append data
   fseek(fp, 0, SEEK_END);

   // write data buffer
   nwrt = fwrite(data, nbps/8, nsamps, fp);

   if(nwrt < (size_t)nsamps) {
      log_printf( "wav_write: error writing data to %s\n", file->name);
	  nsamps = (int)nwrt;
   }

   // force output to file
   //fflush(fp);

   if(verbose_level > 1) {
      log_printf( "wav_write: %ld samples written\n", nwrt);
   }

   // close file
   fclose(fp);

   return(nwrt);
}

/*
 * Write header information to a text file
 */
int wav_create_header_file(wav_file_t *file, u_int32_t sec, u_int32_t usec)
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
	fprintf(fp, "%% WISPR DATA FILE HEADER\n");
	fprintf(fp, "file name: %s\n", file->name);
	fprintf(fp, "file description: %s\n", file->description);
	fprintf(fp, "compression level: %d\n", 0);
	fprintf(fp, "start time sec: %u\n", (unsigned int)sec);
	fprintf(fp, "start time usec: %u\n", (unsigned int)usec);
	fprintf(fp, "samples per second: %d\n", file->fs);
	fprintf(fp, "total samples: %u\n", (unsigned int)file->nsamps);
	fprintf(fp, "bits per sample: %d\n", file->nbps);
	fprintf(fp, "channels: %d\n", file->nchans);

	fclose(fp);

	return(nwrt);
}




