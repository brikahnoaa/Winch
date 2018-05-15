/*
 * Decimation function using FIR and IIR filters from the Blackfin DSP library 
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
 * Embedded Ocean Systems (EOS), 2015 
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

#include "decimate.h"

//#include <cycle_count.h>

// fmaxf is not defined in uclinux math lib, so this needs to be defined here
float fmaxf(float x, float y)
{
   if(x >= y) return(x);
   return(y);
}

//-------------------------------------------------------------------------------
// IIR Decimation using bf dsp functions
//
// filter coeffs are calculated in matlab using iir_coeffs.m
// which is basically this:
//     nfilt = 2;
//     rip = .05;	% passband ripple in dB
//     [b,a] = cheby1(nfilt, rip, 0.8/r);
//
// where r is the decimation factor of 2, 3, 4, and 5
//
#define IIR_NSTAGES 2
static float iir_b_2[2*IIR_NSTAGES+1] = {0.058379, 0.233515, 0.350272, 0.233515, 0.058379, };
static float iir_a_2[2*IIR_NSTAGES] = {-0.662831, 0.829770, -0.310336, 0.082847, };
static float iir_b_3[2*IIR_NSTAGES+1] = {0.016070, 0.064278, 0.096417, 0.064278, 0.016070, };
static float iir_a_3[2*IIR_NSTAGES] = {-1.831081, 1.766034, -0.856023, 0.179666, };
static float iir_b_4[2*IIR_NSTAGES+1] = {0.006113, 0.024452, 0.036678, 0.024452, 0.006113, };
static float iir_a_4[2*IIR_NSTAGES] = {-2.417349, 2.553250, -1.310743, 0.273216, };
static float iir_b_5[2*IIR_NSTAGES+1] = {0.002812, 0.011248, 0.016871, 0.011248, 0.002812, };
static float iir_a_5[2*IIR_NSTAGES] = {-2.762818, 3.125716, -1.670511, 0.352863, };

static float iir_group_delay_2 = 4.287062;
static float iir_group_delay_3 = 4.346275;
static float iir_group_delay_4 = 7.003548;
static float iir_group_delay_5 = 4.463326;

static void iir_filter_coeffs(fract16 *coeffs, float *a_coeffs, float *b_coeffs, float scale)
{
   int i;
   float a[(2 * IIR_NSTAGES)]; // local coeffs
   float b[(2 * IIR_NSTAGES) + 1]; // local coeffs

   /* Initialize local coeffs and delay line */
   for (i = 0; i < (2*IIR_NSTAGES); i++) a[i] = scale * a_coeffs[i];
   for (i = 0; i < (2*IIR_NSTAGES+1); i++) b[i] = b_coeffs[i];

   /* Convert coefficients for DF1 IIR filter*/
   coeff_iirdf1_fr16 (a, b, coeffs, IIR_NSTAGES);

   //fprintf(stdout, "iir_coeffs = [ \n");
   //for(i = 0; i < ((4 * IIR_NSTAGES) + 2); i++) fprintf(stdout, "%d ", coeffs[i]);
   //fprintf(stdout, "]\n");
}

/*
function implements a direct form I infinite impulse response (IIR) filter. 
It generates the filtered response of the input data and stores the result in the output vector. 
The number of input samples and the length of the output vector is specified by the nsamples.
*/
static void iir_filter(fract16 *input, fract16 *output, int nsamples, fract16 *coeffs)
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

//------------------------------------------------------------------
// IIR filter decimation.  

static fract16 ibuf[DECIMATE_FILTER_WINDOW_SIZE];  // filter input buffer
static fract16 obuf[DECIMATE_FILTER_WINDOW_SIZE];  // filter output buffer

int decimate_iir_int16(int16_t *buffer, int nsamps, int R, float *delay)
{
	int i, m, n, k;
	fract16 iir_coeffs[4*IIR_NSTAGES+2];
	
	// set filter coeffs and group delay (num of samples)
	if(R == 2) {
		iir_filter_coeffs(iir_coeffs, iir_a_2, iir_b_2, 1.0);
		*delay = iir_group_delay_2;
	} else if(R == 3) {
		iir_filter_coeffs(iir_coeffs, iir_a_3, iir_b_3, 1.0);
		*delay = iir_group_delay_3;
	} else if(R == 4) {
		iir_filter_coeffs(iir_coeffs, iir_a_4, iir_b_4, 1.0);
		*delay = iir_group_delay_4;
	} else if(R == 5) {
		iir_filter_coeffs(iir_coeffs, iir_a_5, iir_b_5, 1.0);
		*delay = iir_group_delay_5;
	} else {
		fprintf(stdout, "decimate_iir_int16: Error, invalid decimation factor.\n");
		*delay = 0.0;
		return(nsamps);
	}
	
	const int wsize = DECIMATE_FILTER_WINDOW_SIZE;
	const int nwins_in_buffer = nsamps / wsize; // number of windows in data buffer

	// filter the input in segments because iir filter can't operate in place on the buffer
	// and allocating a separate output buffer would require too much memory
	for(m = 0, k = 0, n = 0; n < nwins_in_buffer; n++, k += wsize) {

		// copy a window of data into ibuf
		// applying bitshift to scale the word into a fract16
		for(i = 0; i < wsize; i++) {
			ibuf[i] = (fract16)buffer[i+k];
		}
		
		// apply lowpass filter to the window
		iir_filter(ibuf, obuf, wsize, iir_coeffs);

		// copy back into buffer and decimate
		buffer[m++] = ibuf[0];  // first sample is bad because of filter edge effect
		for(i = 1; i < wsize; i+=R, m++) {
			buffer[m] = obuf[i];
		}
	}

	//fprintf(stdout, "nbufs = %d, m = %d\n", n, m);
	return(m);
}

int decimate_iir_int32(int32_t *buffer, int nsamps, int R, int shift, float *delay)
{
	int i, m, n, k;
	fract16 iir_coeffs[4*IIR_NSTAGES+2];
	
	// set filter coeffs and group delay (num of samples)
	if(R == 2) {
		iir_filter_coeffs(iir_coeffs, iir_a_2, iir_b_2, 1.0);
		*delay = iir_group_delay_2;
	} else if(R == 3) {
		iir_filter_coeffs(iir_coeffs, iir_a_3, iir_b_3, 1.0);
		*delay = iir_group_delay_3;
	} else if(R == 4) {
		iir_filter_coeffs(iir_coeffs, iir_a_4, iir_b_4, 1.0);
		*delay = iir_group_delay_4;
	} else if(R == 5) {
		iir_filter_coeffs(iir_coeffs, iir_a_5, iir_b_5, 1.0);
		*delay = iir_group_delay_5;
	} else {
		fprintf(stdout, "decimate_iir_int32: Error, invalid decimation factor.\n");
		*delay = 0.0;
		return(nsamps);
	}	
	
	const int wsize = DECIMATE_FILTER_WINDOW_SIZE;
	const int nwins_in_buffer = nsamps / wsize; // number of windows in data buffer

	for(m = 0, k = 0, n = 0; n < nwins_in_buffer; n++, k += wsize) {

		// copy a window of data into ibuf
		// applying bitshift to scale the word into a fract16
		for(i = 0; i < wsize; i++) {
			ibuf[i] = 0;
			if((i+k) < nsamps) ibuf[i] = (fract16)(buffer[i+k] >> shift);
		}
		
		// apply lowpass filter to the window
		iir_filter(ibuf, obuf, wsize, iir_coeffs);

		// copy back into buffer and decimate
		for(i = 0; i < wsize; i += R, m++) {
			buffer[m] = (fract32)(obuf[i]) << shift;
		}

	}

	return(m);

}

//-------------------------------------------------------------------------------
// FIR Decimation using bfdsp fir filter functions
//
// FIR filter coefficients found in matlab using fir_coeffs.m
// basically this:  h = fir1(30, 1/r);
// for r = [2, 3, 4, 5]
//

#define FIR_NCOEFFS 30

static float fir_h_2[FIR_NCOEFFS] = {
	0.000000, 0.002937, -0.000000, -0.006730, 0.000000, 0.014094, -0.000000, -0.026785, 
	0.000000, 0.049099, -0.000000, -0.096938, 0.000000, 0.315620, 0.500808, 0.315620, 
	0.000000, -0.096938, -0.000000, 0.049099, 0.000000, -0.026785, -0.000000, 0.014094, 
	0.000000, -0.006730, -0.000000, 0.002937, 0.000000, -0.001700 
};

static float fir_h_3[FIR_NCOEFFS] = {
	0.001769, 0.002534, -0.000000, -0.005806, -0.008527, 0.000000, 0.016914, 0.023109, 
	-0.000000, -0.042360, -0.058300, 0.000000, 0.132063, 0.272300, 0.332609, 0.272300, 
	0.132063, 0.000000, -0.058300, -0.042360, -0.000000, 0.023109, 0.016914, 0.000000, 
	-0.008527, -0.005806, -0.000000, 0.002534, 0.001769, 0.000000, };

static float fir_h_4[FIR_NCOEFFS] = {
	-0.002053, -0.002080, 0.000000, 0.004765, 0.009896, 0.009978, -0.000000, -0.018964, 
	-0.036293, -0.034762, 0.000000, 0.068632, 0.153266, 0.223458, 0.250720, 0.223458, 
	0.153266, 0.068632, 0.000000, -0.034762, -0.036293, -0.018964, -0.000000, 0.009978, 
	0.009896, 0.004765, 0.000000, -0.002080, -0.002053, -0.001204, };

static float fir_h_5[FIR_NCOEFFS] = {
	0.001201, 0.002784, 0.004227, 0.003943, -0.000000, -0.008257, -0.018583, -0.025390, 
	-0.021235, 0.000000, 0.039588, 0.091889, 0.145099, 0.184903, 0.199661, 0.184903, 
	0.145099, 0.091889, 0.039588, 0.000000, -0.021235, -0.025390, -0.018583, -0.008257, 
	-0.000000, 0.003943, 0.004227, 0.002784, 0.001201, 0.000000, };

static float fir_group_delay = 15.0;

static void fir_filter(fract16 *input, fract16 *output, int nsamples, float *coeffs)
{
   int i;
   fir_state_fr16 state; // filter state
   fract16 d[FIR_NCOEFFS];
   fract16 h[FIR_NCOEFFS];
   for(i = 0; i < FIR_NCOEFFS; i++) {
      d[i]=0;
      h[i] = float_to_fr16(coeffs[i]);
   }
   fir_init(state, h, d, FIR_NCOEFFS, 1); // initialize filter
   fir_fr16(input, output, nsamples, &state);
}

int decimate_fir_int16(int16_t *buffer, int nsamps, int R, float *delay)
{
	int i, j;
	float *fir_coeffs = fir_h_2;

	if(R == 2) {
		fir_coeffs = fir_h_2;
	} else if(R == 3) {
		fir_coeffs = fir_h_3;
	} else if(R == 4) {
		fir_coeffs = fir_h_4;
	} else if(R == 5) {
		fir_coeffs = fir_h_5;
	} else {
		fprintf(stdout, "decimate_fir_int16: Error, invalid decimation factor.\n");
		return(nsamps);
	}
	*delay = fir_group_delay;

	// perform the FIR filtering in place (ibuf = obuf)
	fract16 *ibuf = (fract16 *)buffer;
	fract16 *obuf = ibuf;
	fir_filter(ibuf, obuf, nsamps, fir_coeffs);

	// decimate by R
	for(i = 0, j = 0; j < nsamps; i++, j+=R) {
		obuf[i] = obuf[j];
	}

	return(i);
}

int decimate_fir_int32(int32_t *buffer, int nsamps, int R, int shift, float *delay)
{
	int i, j;
	float *fir_coeffs = fir_h_2;

	if(R == 2) {
		fir_coeffs = fir_h_2;
	} else if(R == 3) {
		fir_coeffs = fir_h_3;
	} else if(R == 4) {
		fir_coeffs = fir_h_4;
	} else if(R == 5) {
		fir_coeffs = fir_h_5;
	} else {
		fprintf(stdout, "decimate_fir_int32: Error, invalid decimation factor.\n");
		return(nsamps);
	}
	*delay = fir_group_delay;

	// overwrite buffer with shifted fract16 values
	fract16 *ibuf = (fract16 *)buffer;
	for(i = 0; i < nsamps; i++) {
		ibuf[i] = (fract16)(buffer[i] >> shift);
	}

	// perform the FIR filtering in place (ibuf = obuf)
	fract16 *obuf = ibuf;
	fir_filter(ibuf, obuf, nsamps, fir_coeffs);

	// decimate by R
	for(i = 0, j = 0; j < nsamps; i++, j+=R) {
		buffer[i] = (fract32)(obuf[j]) << shift;
	}

	return(i); 

}

