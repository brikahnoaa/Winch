/*
*  wispr_flac.h:
*/
#ifndef _WISPR_FLAC_H
#define _WISPR_FLAC_H

//#include "FLAC/metadata.h"
//#include "FLAC/stream_encoder.h"
#include "FLAC/metadata.h"
#include "FLAC/stream_encoder.h"

// size of pcm buffer passed to flac encloder
// this should always be less than and divide evenly into the data buffer size (data->nsamps)
// used in wispr_flac_write()
#define WISPR_FLAC_BUF_SIZE 4096

typedef struct {
   int status;
   u_int32_t sec;         // second start time
   u_int32_t usec;        // usecond start time
   int compression;       // compression level
   int nbps;              // sample size [bits]
   int nchans;            // num of channels
   int fs;                // sampling freq [hz]
   u_int32_t nsamps;      // total num of samples in file
} wispr_flac_info_t;


typedef struct {
   char path[32];
   char prefix[16];
   char name[64];             /* file name */
   char description[128];     /* description string */
   wispr_flac_info_t info;
   FLAC__StreamEncoder *encoder;         /* encoder pointer */
   FLAC__StreamEncoderState encoder_state;
   int use_metadata;          /* write metadata */
   FLAC__StreamMetadata *metadata[2];    /* meta data pointers */
   int check_min_bfree;       /* check file system stats on each file open */
   unsigned long bsize;	      /* block size (use stat.f_bsize for optimal transfer) */
   unsigned long bfree;	      /* current free blocks in fs */
   unsigned long bfree_min;   /* min free blocks to leave in fs */
   unsigned long blocks;      /* total number of blocks in fs */
} wispr_flac_t;


/* function prototypes */
extern int wispr_flac_create(wispr_flac_t *file, char *path, char *prefix);
extern void wispr_flac_delete(wispr_flac_t *file);
extern FLAC__bool wispr_flac_init_file(wispr_flac_t *file, u_int32_t sec, u_int32_t usec);
extern void wispr_flac_finish(wispr_flac_t *file);
extern int wispr_flac_write(wispr_flac_t *file, char *data, u_int32_t nsamps, int nbps);
extern int wispr_flac_write2(wispr_flac_t *file, char *data, u_int32_t nsamps, int nbps);
extern int wispr_flac_write_int16(wispr_flac_t *file, int16_t *data, u_int32_t nsamps);
extern int wispr_flac_write_int32(wispr_flac_t *file, int32_t *data, u_int32_t nsamps);
extern int wispr_flac_create_header_file(wispr_flac_t *file, u_int32_t sec, u_int32_t usec);

extern int wispr_flac_set_compression_level(wispr_flac_t *file, int comp_level); 
extern int wispr_flac_set_bits_per_sample(wispr_flac_t *file, int nbps); 
extern int wispr_flac_set_channels(wispr_flac_t *file, int nchans); 
extern int wispr_flac_set_sample_rate(wispr_flac_t *file, int fs);
extern int wispr_flac_set_total_samples(wispr_flac_t *file, int nsamps); 
extern int wispr_flac_set_total_samples_estimate(wispr_flac_t *file, int nsamps); 
extern int wispr_flac_set_min_free_blocks(wispr_flac_t *file, int bfree_min);
extern int wispr_flac_set_description(wispr_flac_t *file, char *str);
extern int wispr_flac_use_metadata(wispr_flac_t *file);

#endif /* _FLAC_FILE_H */

