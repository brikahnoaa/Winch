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

//#include <cycle_count.h>

int verbose_level = 1;

//-------------------------------------------------------------------------------
// filters using bfdsp functions
//

// IIR filter coefs defined by Holger for the OR data
/* Filter coeffs found in matlab as
fs = 93750;
N = 2;
%[b1,a1]=butter(N, [43000 46000]/(fs/2));
[b1,a1]=butter(N, [40000 43000]/(fs/2));
[b2,a2]=butter(N, [5000 8000]/(fs/2));

% print c version, cut/paste into code 
fprintf('%s\n%s\n%s\n%s\n%s\n', ...
sprintf('#define IIR_NSTAGES %d',N), ...
['float iir_b_high[2*IIR_NSTAGES+1] = {', sprintf('%f, ', b1), '};'], ...
['float  iir_b_low[2*IIR_NSTAGES+1] = {', sprintf('%f, ', b2), '};'], ...
['float iir_a_high[2*IIR_NSTAGES] = {', sprintf('%f, ', a1(2:end)), '};'], ...
['float  iir_a_low[2*IIR_NSTAGES] = {', sprintf('%f, ', a2(2:end)), '};'])

*/

#define IIR_NSTAGES 2
float iir_b_high[2*IIR_NSTAGES+1] = {0.008826, 0.000000, -0.017652, 0.000000, 0.008826, };
float iir_a_high[2*IIR_NSTAGES] = {3.496270, 4.786734, 3.030722, 0.752516, };
float iir_b_low[2*IIR_NSTAGES+1] = {0.008826, 0.000000, -0.017652, 0.000000, 0.008826, };
float iir_a_low[2*IIR_NSTAGES] = {-3.387136, 4.598097, -2.936119, 0.752516, };

//#define IIR_NSTAGES 3
//float iir_b_high[2*IIR_NSTAGES+1] = {0.000839, 0.000000, -0.002517, 0.000000, 0.002517, 0.000000, -0.000839, };
//float iir_a_high[2*IIR_NSTAGES] = {5.265770, 11.861566, 14.607580, 10.369391, 4.024674, 0.668440, };
//float iir_b_low[2*IIR_NSTAGES+1] = {0.000839, 0.000000, -0.002517, 0.000000, 0.002517, 0.000000, -0.000839, };
//float iir_a_low[2*IIR_NSTAGES] = {-5.101401, 11.292308, -13.827592, 9.871870, -3.899045, 0.668440, };

fract16 iir_high_coeffs[4*IIR_NSTAGES+2];
fract16 iir_low_coeffs[4*IIR_NSTAGES+2];

// fmaxf is not defined in uclinux math lib, so this needs to be defined here
float fmaxf(float x, float y)
{
   if(x >= y) return(x);
   return(y);
}

void iir_filter_coeffs(fract16 *coeffs, float *a_coeffs, float *b_coeffs, float scale)
{
   int i;
   float a[(2 * IIR_NSTAGES)]; // local coeffs
   float b[(2 * IIR_NSTAGES) + 1]; // local coeffs

   /* Initialize local coeffs and delay line */
   for (i = 0; i < (2*IIR_NSTAGES); i++) a[i] = scale * a_coeffs[i];
   for (i = 0; i < (2*IIR_NSTAGES+1); i++) b[i] = b_coeffs[i];

   /* Convert coefficients for DF1 IIR filter*/
   coeff_iirdf1_fr16 (a, b, coeffs, IIR_NSTAGES);

   if(verbose_level > 3) {
	   log_printf("iir_coeffs = [ \n");
	   for(i = 0; i < ((4 * IIR_NSTAGES) + 2); i++) log_printf("%d ", coeffs[i]);
	   log_printf("]\n");
   }

}

void iir_filter(fract16 *input, fract16 *output, int nsamples, fract16 *coeffs)
{
   int i;
   fract16 delay[(4 * IIR_NSTAGES) + 2]; // delay line
   iirdf1_state_fr16 state; // filter state
   //iir_state_fr16 state; // filter state

   /* Initialize delay line */
   for (i = 0; i < (4*IIR_NSTAGES+2); i++) delay[i] = 0;

   /* Initialize filter description */
   iirdf1_init (state, coeffs, delay, IIR_NSTAGES);
   //iir_init (state, coeffs, delay, IIR_NSTAGES);

   /* Call the function */
   iirdf1_fr16 (input, output, nsamples, &state);
   //iir_fr16 (input, output, nsamples, &state);
}


int main(int argc, char **argv)
{
	int i, n;

	char wave_test_file[32] = "/mnt/kw_test.wav";
	FILE *wave_fp = NULL;
	struct WaveFile wave;

	short *in_buffer;
	short *buffer1;
	short *buffer2;

	int num_buffers = 1824;
	int nsamps = 512; 
	int shift = 0;
	float gain = 1.0;

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
	size_t nbytes = 2*nsamps; // always use 4 byte words for adc read buffer

	// set non-kernel physical memory pointer after last adc buffer
	in_buffer = (short *)malloc(nbytes);
	buffer1 = (short *)malloc(nbytes);
	buffer2 = (short *)malloc(nbytes);

	char ofile[] = "/mnt/filter_test.txt";
	FILE *out_fp = fopen(ofile, "w");
	fclose(out_fp);

	out_fp = fopen(ofile, "w");
	
	iir_filter_coeffs(iir_high_coeffs, iir_a_high, iir_b_high, 1.0);
	iir_filter_coeffs(iir_low_coeffs, iir_a_low, iir_b_low, 1.0);

	for( n = 0; n < num_buffers; n++ ) {

		// read test data into buffer
		int nsize = wave.nsize;
		int nrd = fread(in_buffer, nsize, nsamps, wave_fp);

		if(nrd != nsamps) {
			fprintf(stdout, "Error reading wave test file (%d).\n", nrd);
			return(0);
		}

		fract16 buf_max = 0;
		float buf_mean = 0;
		for(i=0; i<nsamps; i++) {
			//in_buffer[i] = in_buffer[i] >> shift;
			buf_mean += fr16_to_float(in_buffer[i]);
			buf_max = max_fr1x16(buf_max, (fract16)(in_buffer[i]));
		}
		buf_mean /= (float)nsamps;

		//fprintf(stdout, "max = %f (%d), mean = %f\n", fr16_to_float(buf_max), buf_max, buf_mean);

		//cycle_t start_count;
		//cycle_t final_count;
		//START_CYCLE_COUNT(start_count);

		iir_filter(in_buffer, buffer1, nsamps, iir_high_coeffs);
		
		iir_filter(in_buffer, buffer2, nsamps, iir_low_coeffs);

		fract16 buf1_max = 0;
		float buf1_mean = 0;
		for(i=0; i<nsamps; i++) {
			//in_buffer[i] = in_buffer[i] >> shift;
			buf1_mean += fr16_to_float(in_buffer[i]);
			buf1_max = max_fr1x16(buf1_max, (fract16)(buffer2[i]));
		}
		buf1_mean /= (float)nsamps;

		//STOP_CYCLE_COUNT(final_count,start_count);
		//PRINT_CYCLES("detect: number of cycles = ",final_count);

		for(i=0; i<nsamps; i++) {
			fprintf(out_fp, "%d %d %d\n", in_buffer[i], buffer1[i], buffer2[i]);
		}

		//STOP_CYCLE_COUNT(final_count,start_count);
		//PRINT_CYCLES("detect: number of cycles = ",final_count);

	}
	
	fclose(out_fp);
	return(0);

}
