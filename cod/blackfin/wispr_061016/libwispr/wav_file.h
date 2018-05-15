/*
*  wav_file.h:  data file utility
*/
#ifndef _WAV_FILE_H
#define _WAV_FILE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
//#include "wispr.h"

#define WISPR_WAV_BUF_SIZE 4096

// standard/simple wav file header
typedef struct wav_file_header {
  char ChunkID[4];
  int ChunkSize;
  char Format[4];
  char Subchunk1ID[4];
  int Subchunk1Size;
  short AudioFormat;
  short NumChannels;
  int SampleRate;
  int ByteRate;
  short BlockAlign;
  short BitsPerSample;
  char Subchunk2ID[4];
  int Subchunk2Size;
} wav_file_header_t;

//  wav file type
typedef struct {
   char path[32];
   char prefix[16];
   char name[64];      /* current data file name */
   char description[128];     /* description string */
   u_int32_t sec;     // second start time
   u_int32_t usec;    // usecond start time
   int nchans;        //
   int nbps;          // number bits per sample
   int fs;            //
   u_int32_t nsamps;   //
   wav_file_header_t hdr;
} wav_file_t;

/* function prototypes */
extern int wav_create(wav_file_t *file, char *path, char *prefix);
extern void wav_close(wav_file_t *file);
extern int wav_open(wav_file_t *file, u_int32_t sec);
extern size_t wav_write(wav_file_t *file, char *data, u_int32_t nsamps);

extern size_t wav_read_header(FILE *fp, wav_file_header_t *wav);
extern size_t wav_write_header(FILE *fp, wav_file_header_t *wav);

extern int wav_set_bits_per_sample(wav_file_t *file, int nbps); 
extern int wav_set_channels(wav_file_t *file, int nchans); 
extern int wav_set_sample_rate(wav_file_t *file, int fs);
extern int wav_set_total_samples_estimate(wav_file_t *file, int nsamps); 
extern int wav_set_total_samples(wav_file_t *file, int nsamps); 
extern int wav_set_description(wav_file_t *file, char *str);

extern int wav_create_header_file(wav_file_t *file, u_int32_t sec, u_int32_t usec);

#endif /* _WAV_file_H */

