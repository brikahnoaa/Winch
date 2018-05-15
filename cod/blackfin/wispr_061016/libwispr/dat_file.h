/*
*  dat_file.h:  data file utility
*/

#ifndef _DAT_FILE_H
#define _DAT_FILE_H

#define DAT_FILE_BUF_SIZE 4096

typedef struct {
   u_int32_t sec;         // second start time
   u_int32_t usec;        // usecond start time
   int status;
   int compression;       // compression level
   int nbps;              // sample size [number of bits]
   u_int32_t nsamps;      // total num of samples in file
   //u_int32_t nsamps_est;  // estimate total num of samples in file
   int nchans;            // num of channels
   int fs;                // sampling freq [hz]
   int gain;              // pre-amp gain
   int shift;             // data word bit shift (only applies to 16 bps data) [shifted bits]
} dat_file_info_t;


typedef struct {
   char log[32];       // log file name
   char path[32];
   char prefix[16];
   char name[64];      /* current data file name */
   unsigned long bsize;	      /* block size (use stat.f_bsize for optimal transfer) */
   unsigned long bfree;	      /* current free blocks in fs */
   unsigned long bfree_min;   /* min free blocks to leave in fs */
   unsigned long blocks;      /* total number of blocks in fs */
   dat_file_info_t info;
} dat_file_t;


/* function prototypes */
extern int dat_create(dat_file_t *file, char *path, char *prefix);
extern void dat_close(dat_file_t *file);
extern int dat_init_new(dat_file_t *file, u_int32_t sec);
extern int dat_write_header(dat_file_t *file, u_int32_t sec, u_int32_t usec);
extern int dat_write(dat_file_t *file, char *data, u_int32_t nsamps, int nbytes);
extern int dat_write_int32(dat_file_t *file, u_int32_t *data, u_int32_t nsamps);

extern int dat_set_compression_level(dat_file_t *file, int comp_level); 
extern int dat_set_bits_per_sample(dat_file_t *file, int nbps); 
extern int dat_set_channels(dat_file_t *file, int nchans); 
extern int dat_set_sample_rate(dat_file_t *file, int fs);
extern int dat_set_total_samples_estimate(dat_file_t *file, int nsamps); 
extern int dat_set_total_samples(dat_file_t *file, int nsamps); 
extern int dat_set_gain(dat_file_t *file, int gain);
extern int dat_set_bitshift(dat_file_t *file, int shift);
extern int dat_set_log_filename(dat_file_t *file, char *name); 

#endif /* _DAT_dat_file_H */

