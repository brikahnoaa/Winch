/*
 * sport_adc.h
 * ------
 * Embedded Ocean Systems 2013
 * 
 */

#ifndef _SPORT_ADC_H
#define _SPORT_ADC_H

#include <asm/bfin_sport.h>
#include <sys/types.h>
//#include <fract.h>

//#include "wispr.h"

// ADC BLOCK SIZE is the number of 32 bits words in a data block
#define ADC_DATA_BLOCK_SIZE 4096

// status flags
#define ADC_DATA_CLIPPED 0x01
#define ADC_BUFFER_OVERRUN 0x02

// adc time series data type
typedef struct {
   struct timeval time;   // start time
   int status;            // data status
   int nbps;              // number of bits per sample [bits]
   int nsamps;            // num of samples per blk [samples]
   int nchans;            // num of channels
   int fs;                // sampling freq [hz]
   int gain;              // pre-amp gain
   int scale;             // data word scaling [shifted bits]
   void *buffer;
} sport_adc_data_t;


// sport adc parameters
struct sport_adc_struct {
   char device[32];  // file name
   int fd;           // file descriptor for sport device
   int mclk_fd;      // file descriptor for master clock time (convert clock)
   int rsclk_fd;     // file descriptor for receive dta clock time (rcv clock)
   int nbufs;        // number of dma buffers
   int nbps;         // actual number of bits per sample (adc resolution) 
   int nsize;        // buffer sample size in bytes (always 4 bytes for sport bus dma transfers)
   int nchans;       // buffer num of channels
   int nsamps;       // buffer num of samples
   int fs;           // buffer sampling freq [hz]
   int gain;         // pre-amp gain 
   int status;        // status flags 
   struct timespec time; // sample start time
   struct sport_dma_buffer dma_buffer; // dma buffer
   char *buffer[SPORT_MAX_NUM_DMA_BUFS];  // dma data buffers
};

typedef struct sport_adc_struct sport_adc_t;

extern int sport_ad7766_open(sport_adc_t *adc, int fs);
extern int sport_ad7766_start_clock(sport_adc_t *adc);

extern int16_t *sport_ad7766_reformat_int16(u_int32_t *buf, int nsamps, int shift);
extern unsigned char *sport_ad7766_reformat_int24(u_int32_t *buf, int nsamps);
extern int32_t *sport_ad7766_reformat_int32(u_int32_t *buf, int nsamps);

extern int sport_ad7988_open(sport_adc_t *adc, int fs);
extern int sport_ad7988_start_clock(sport_adc_t *adc);

extern int sport_adc_alloc_pmem_buffers(sport_adc_t *adc, float adc_duration, unsigned long pmem_start, unsigned long pmem_end);
extern int sport_adc_malloc_buffers(sport_adc_t *adc, float adc_duration);
extern int sport_adc_start_dma(sport_adc_t *adc);
extern int sport_adc_pause_dma(sport_adc_t *adc);
extern int sport_adc_resume_dma(sport_adc_t *adc);
extern int sport_adc_read_dma_data(sport_adc_t *adc, sport_adc_data_t *data);
extern int sport_adc_read_dma(sport_adc_t *adc, u_int32_t **buffer);
//extern int sport_adc_read(sport_adc_t *adc, u_int32_t *buf, int nsamps);

extern void sport_adc_close(sport_adc_t *adc);
extern int sport_adc_set_number_of_dma_buffers(sport_adc_t *adc, int nbufs);

#endif /* _SPORT_ADC_H */

