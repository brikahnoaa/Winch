/*
 
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>

#include <math_bf.h>
#include <math.h>
#include <fract.h>
#include <fract2float_conv.h>
#include <filter.h>
#include <vector.h>

#include "wispr.h"
#include "detect.h"

//#include <cycle_count.h>
#define BFIN_PMEM_START 0x3000000   /* use when mem=48M */
#define BFIN_PMEM_END 0x3F00000     /* use when max_mem=64M */
#define BFIN_PMEM_SIZE (BFIN_PMEM_END - BFIN_PMEM_START)

int verbose_level = 4;

// Detection parameters
static int min_clicks_per_detection = 10;		// min number of clicks for a detection, see detect.h
static int num_bufs_per_detection = 6;			// number of adc bufs that define the detection duration
static float detection_ici_thresh = 0.33;
static float detection_ici_min = 0.005;
static float detection_ici_max = 0.25;
static float detection_thresh_factor = 170.0;

extern float *dtx_ratio;  // defined in detect.c

static sport_adc_t adc;          // adc object
static int adc_fs = 93750;              // sampling freq Hz
static float adc_duration = 10.0;        // duration of each adc buffer (seconds)
static int adc_gain = 0;                 // pre amp gain
static int bitshift = 8;                 // bit shift
static u_int32_t *adc_buffer;

int main(int argc, char **argv)
{
	int i, n;

	char wave_test_file[32] = "/mnt/kw_test.wav";
	FILE *wave_fp = NULL;
	struct WaveFile wave;

	double t0 = 0.0;

	// open wave file and read header
	wave_fp = fopen(wave_test_file, "r");
	if(wave_fp == NULL) {
		log_printf("Error openning wave test file %s\n", wave_test_file);
		return(-1);
	}

	// read wave file header
	// this sets the wave.nsaps to the number of samps in file
	wave_read_header(wave_fp, &wave);

	if(verbose_level > 0) 
		log_printf("wave_test_init: Subchunk2Size %d / %d BitsPerSample = %d samples\n", 
		wave.Subchunk2Size, wave.BitsPerSample, wave.nsamps);

	// assign data buffer using available phys mem
	size_t nbytes = 2*adc.nsamps; // always use 4 byte words for adc read buffer

	// open ad7766 device
	if(sport_ad7766_open(&adc, adc_fs) < 0) {
		log_printf("Error openning sport ad7766\n");
		return(0);
	}

	// allocate adc dma buffers
	if(sport_adc_alloc_pmem_buffers(&adc, adc_duration, BFIN_PMEM_START, BFIN_PMEM_END) < 0) {
		log_printf("Error allocating sport adc dma buffers\n");
		return(0);
	}

	// set non-kernel physical memory pointer
	adc_buffer = (u_int32_t *)adc.buffer[0];
	
	// set non-kernel physical memory pointer after last adc buffer
	nbytes = 2*adc.nsamps; // always use 4 byte words for adc read buffer
	short *wave_buffer = (short *)((void *)adc.buffer[adc.nbufs - 1] + nbytes);

	//char ofile[] = "/mnt/filter_test.txt";
	//FILE *out_fp = fopen(ofile, "w");
	//fclose(out_fp);
	//out_fp = fopen(ofile, "w");

	if(init_detect(min_clicks_per_detection, adc.nsamps, num_bufs_per_detection, bitshift, 
		detection_ici_thresh, detection_thresh_factor, detection_ici_min, detection_ici_max) <= 0) {
		fprintf(stdout, "Error initializing click detection\n");
	}

	// loop until the wave file is empty
	while(1) {

		for( n = 0; n < num_bufs_per_detection; n++ ) {

			// read test data into buffer
			int nsize = wave.nsize;
			int nrd = fread(wave_buffer, nsize, adc.nsamps, wave_fp);

			// quit at end of file
			if(nrd != adc.nsamps) {
				fprintf(stdout, "Error reading wave test file (%d).\n", nrd);
				return(0);
			}

			for (i = 0; i < adc.nsamps; i++) {
				adc_buffer[i] = (u_int32_t)(wave_buffer[i]) << bitshift;
			}

			//cycle_t start_count;
			//cycle_t final_count;
			//START_CYCLE_COUNT(start_count);

			// run detect on data buffer
			int status;
			int ndtx = detect(adc_buffer, adc.nsamps, adc.fs, t0, &status);
			t0 += ((double)(adc.nsamps) / (double)(adc.fs));

			//STOP_CYCLE_COUNT(final_count,start_count);
			//PRINT_CYCLES("detect: number of cycles = ",final_count);

			//STOP_CYCLE_COUNT(final_count,start_count);
			//PRINT_CYCLES("detect: number of cycles = ",final_count);

		}
	}

	fclose(wave_fp);

	return(0);

}
