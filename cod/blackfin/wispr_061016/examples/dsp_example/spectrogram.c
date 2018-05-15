/*
 * WISPR spectrogram function
 *
 * USAGE:
 * int spectrogram_init(spectrogram_t *psd, int fft_size, int overlap, int fs)
 * int spectrogram(spectrogram_t *psd, u_int32_t *input, int nsamps, int navg, int scaling_method, int shift)
 * 
 * DESCRIPTION:
 *
 * Spectrogram using a Short-Time Fourier Transform (STFT) to calculate the spectrogram 
 * of the signal specified by the vector 'input'. 
 * 
 * The spectrogram is formed by dividing the input into overlapping segments of length equal, 
 * multiplying the segment by a Hamming window of equal length, performing an FFT, and then
 * averaging the magnitude of the FFT over a specified number of time bins.
 * 
 * The number of frequency points used to calculate the discrete Fourier transforms is fft_size.
 * fft_size must be a power of 2 greater than 8 and less than MAX_FFT_POW2 as defined in spectrogram.h. 
 *  
 * The specified segment overlap must be an integer smaller than fft_size.   
 * Fs is the sampling frequency of the input signal specified in Hz.
 * The number of overlaping segments to average to form each spectral time bin is specified by navg. 
 * A value of navg=1 specified no time averaging.
 *
 * The averaged magnitude of the spectrogram is then saved in the psd structure (psd.magnitude) 
 * as a vector of length num_freq_bins * num_time_bins, where
 * num_freq_bins = (fft_size/2 + 1), 
 * num_time_bins = ((nsamps - overlap)/(fft_size - overlap)) / navg
 * 
 * The spectrogram function also defines frequency and time vectors (psd.freq and psd.time)
 * that specify the time (seconds) and frequency (Hz) of each spectrogram bins. 
 * psd.freq has length num_freq_bins and units of Hz.
 * psd.time has length num_time_bins and units of seconds.
 * 
 * The argument scale_method controls how the function will apply scaling
 * while computing a Fourier Transform. The available options are static
 * scaling (dividing the input at any stage by 2), dynamic scaling (dividing
 * the input at any stage by 2 if the largest absolute input value is greater or
 * equal than 0.5), or no scaling.
 * If static scaling is selected, the function will always scale intermediate
 * results, thus preventing overflow. The loss of precision increases in line
 * with fft_size and is more pronounced for input signals with a small magnitude
 * (since the output is scaled by 1/fft_size). To select static scaling,
 * set the argument scale_method to a value of 1. The block exponent
 * returned will be log2(fft_size) depending upon the number of times
 * that the function scales the intermediate set of results.
 * If dynamic scaling is selected, the function will inspect intermediate
 * results and only apply scaling where required to prevent overflow. The loss
 * of precision increases in line with the size of the FFT and is more pronounced
 * for input signals with a large magnitude (since these factors
 * increase the need for scaling). The requirement to inspect intermediate
 * results will have an impact on performance. To select dynamic scaling, set
 * the argument scale_method to a value of 2. The block exponent returned
 * will be between 0 and log2(fft_size).
 * If no scaling is selected, the function will never scale intermediate results.
 * There will be no loss of precision unless overflow occurs and in this case
 * the function will generate saturated results. The likelihood of saturation
 * increases in line with the fft_size and is more pronounced for input signals
 * with a large magnitude. To select no scaling, set the argument
 * scale_method to 3. The block exponent returned will be 0.
 * --------------------------------------------------------------------------------
 * Here's and example of how to use the spectrogram functions:
 * 
 * spectrogram_t psd;
 * int psd_fft_size = 256;		// fft size
 * int psd_overlap = 128;		// 50% fft overlap
 * int psd_time_average = 3;    // average 3 ffts segments for each time bin
 * int psd_scaling_method = 1;  // static scaling
 *
 * // initialize the spectrogram
 * if(spectrogram_init(&psd, psd_fft_size, psd_overlap, adc_fs) < 0) {
 *   log_printf("Error initializing spectrogram\n");
 *   return(0);
 * }
 * 
 * int16_t *buffer = ...  // read data into int16 buffer
 *
 * // build spectrogram from the adc data buffer
 * spectrogram(&psd, buffer, nsamps, psd_time_average, psd_scaling_method);
 * 
 * // save the spectrogram as a pgm image file
 * spectrogram_write_pgm(&psd, filename);
 *
 * -------------------------------------------------------------------------------- 
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
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/rtc.h>
#include <fract.h>
#include <filter.h>

#include <cycle_count.h>
#include <time.h>

#include "spectrogram.h"
#include "log.h"

extern int verbose_level;

//
// initialize the spectrogram
//
int spectrogram_init(spectrogram_t *psd, int fft_size, int overlap, int fs)
{
	// check input args
	int n;
	int fft_size_okay = 0;
	int pow2 = 4;
	for (n = 2; n < MAX_FFT_POW2; n++) {
		pow2 *= 2;
		if(fft_size == pow2) fft_size_okay = 1; 
	}
	if(fft_size_okay == 0) {
		log_printf("spectrogram: invalid fft size\n");
		return(-1);
	}
	if((overlap < 0) || (overlap > fft_size)) {
		log_printf("spectrogram: invalid overlap size\n");
		return(-1);
	}

	// allocate fft twiddle array
	complex_fract16 *twid = (complex_fract16 *)malloc(fft_size*sizeof(complex_fract16));
	if(twid == NULL) {
		log_printf("spectrogram: error allocating twiddle table\n");
		return(-1);
	}
	twidfftrad2_fr16(twid, fft_size);

	// allocate fft window array
	//if(win != NULL) free(win);
	fract16 *win = (fract16 *)malloc(fft_size*sizeof(fract16));
	if(win == NULL) {
		log_printf("spectrogram_init: error allocating window\n");
		return(-1);
	}
	gen_hamming_fr16(win, 1, fft_size);

	// allocate fft buffers
	complex_fract16 *obuf = (complex_fract16 *)malloc(fft_size*sizeof(complex_fract16));
	if(obuf == NULL) {
		log_printf("spectrogram_init: error allocating fft output buffer\n");
		return(-1);
	}

	//fract16 *ibuf = (fract16 *)malloc(fft_size*sizeof(fract16));
	fract16 *ibuf = (fract16 *)obuf;  // use obuf for ibuf
	if(ibuf == NULL) {
		log_printf("spectrogram_init: error allocating fft input buffer\n");
		return(-1);
	}

	// initialize psd structure for return
	psd->nfft = fft_size;
	psd->overlap = overlap;
	psd->twiddle_table = twid;
	psd->window = win;
	psd->scaling_method = 1;
	psd->fs = fs;
	psd->navg = 1;

	// save the fft buffer to use when building the spectrogram
	psd->obuf = obuf;
	psd->ibuf = ibuf;

	// don't alloc magnitude yet because it's size depend on nsamps
	psd->magnitude = NULL;
	psd->num_time_bins = 0;
	psd->num_freq_bins = 0;

	if(verbose_level > 1) {
		log_printf("spectrogram_init: fft size = %d, overlap = %d\n",
			fft_size, overlap);
	}

	return(1);
}

//
// clear the spectrogram
//
void spectrogram_clear(spectrogram_t *psd)
{
	if(psd->magnitude != NULL) {
		free(psd->magnitude);
		psd->magnitude = NULL;
	}
	if(psd->twiddle_table != NULL) {
		free(psd->twiddle_table);
		psd->twiddle_table = NULL;
	}
	if(psd->ibuf != NULL) {
		free(psd->ibuf);
		psd->ibuf = NULL;
	}
	if(psd->obuf != NULL) {
		free(psd->obuf);
		psd->obuf = NULL;
	}
	if(psd->window != NULL) {
		free(psd->window);
		psd->window = NULL;
	}

}

//
// calculate the spectrogram for the input signal
//
int spectrogram(spectrogram_t *psd, int16_t *input, int nsamps, int navg, int scaling_method)
{
	int n, m, i, j, k;

	// number of spectral estimates in the psd
	int fft_size = psd->nfft;
	int overlap = psd->overlap;

	int skip = fft_size - overlap;
	int num_time_bins = ((nsamps - fft_size)/(fft_size - overlap)) / navg;
	int num_freq_bins = fft_size/2 + 1;

	int smeth = scaling_method;
	if((scaling_method < 1) || (scaling_method > 3)) {
		log_printf("spectrogram: unknown scaling method\n");
		smeth = 1;
	}
	psd->scaling_method = smeth;

	if(navg < 0) navg = 1;
	psd->navg = navg;
	
	fract16 *win = psd->window;  // pre-defined window function
	complex_fract16 *twid = psd->twiddle_table;  // pre-allocated twiddle table 
	int blkexp = 0;  // block exponent returned by fft function

	// use the same buffer for fft input and output
	fract16 *ibuf = psd->ibuf;
	complex_fract16 *obuf = psd->obuf;

	// free previously allocated memory, if the size has changed
	if(((psd->num_time_bins * psd->num_freq_bins) != (num_time_bins*num_freq_bins)) && (psd->magnitude != NULL)) {
		free(psd->magnitude);
		psd->magnitude = NULL;
	}

	// allocate the spectrogram output array, if it hasn't already been allocated 
	if(psd->magnitude == NULL) {
		psd->magnitude = (fract16 *)malloc(num_time_bins*num_freq_bins*sizeof(fract16));	
		if(psd->magnitude == NULL) {
			log_printf("spectrogram: error allocating output buffer\n");
			return(-1);
		}		
		psd->num_time_bins = num_time_bins;
		psd->num_freq_bins = num_freq_bins;
		if(verbose_level > 0) 
			log_printf("spectrogram: %d time bins, %d frequency bins\n", num_time_bins, num_freq_bins );
	}
	fract16 *output = psd->magnitude;

	// define size of time and frequency bins
	float fs = psd->fs;
	float duration = (float)nsamps / fs; // buffer size in seconds
	psd->dfreq = fs / (float)fft_size;
	psd->dtime =  duration / (float)num_time_bins;

	fract16 norm = float_to_fr16(1.0 / (float)navg);

	// build the spectrogram using overlapping ffts 
	int istart = 0;  // start index of segment
	int iend;  // end index of segment
	for (i = 0; i < num_time_bins; i++)  {

		int off = i*num_freq_bins;

		// initialize output vector
		for(j = 0; j < num_freq_bins; j++) output[j+off] = 0;

		// average the time bins
		for(k = 0; k < navg; k++)  {

			iend = istart + fft_size; // end of segment

			// handle buffer at the end of the input signal
			if(iend > nsamps) {
				iend = nsamps;
				for(m = 0; m < fft_size; m++) ibuf[m] = 0.0;
			}

			// load the fft input buffer
			for(n = istart, m = 0; n < iend; n++, m++) {
				fract16 value = (fract16)input[n];
				ibuf[m] = multr_fr1x16( value, win[m] );
			}

			// perform the fft on the buffer
			rfft_fr16(ibuf, obuf, twid, 1, fft_size, &blkexp, smeth);

			// sum and save the magnitude of the complex fft output
			// could take cabf after the sum
			for(n = 0; n < num_freq_bins; n++) {
				output[n+off] += cabs_fr16(obuf[n]);
			}

		    // increment the start index by skip=nsamps-overlap
			istart += skip;

		}

		// normalize the average
		for(j = 0; j < num_freq_bins; j++) output[j+off] = multr_fr1x16( output[j+off], norm );
		
	}

	// initialize psd structure for return
	return(num_time_bins);
}

//
// save spectrogram as an PGM formated image file.
//
// Each PGM image consists of the following:
// A "magic number" for identifying the file type. A pgm image's magic number is the two characters "P5".
// Whitespace (blanks, TABs, CRs, LFs).
// A width, formatted as ASCII characters in decimal.
// Whitespace.
// A height, again in ASCII decimal.
// Whitespace.
// The maximum gray value (Maxval), again in ASCII decimal. Must be less than 65536, and more than zero.
// A single whitespace character (usually a newline).
// The binary image raster of Height rows, in order from top to bottom. Each row consists of Width gray values,
// in order from left to right. Each gray value is a number from 0 through Maxval, with 0 being black
// and Maxval being white. Each gray value is represented in pure binary by either 1 or 2 bytes.
// If the Maxval is less than 256, it is 1 byte. Otherwise, it is 2 bytes. The most significant byte is first.
// A row of an image is horizontal. A column is vertical. The pixels in the image are square and contiguous.
// Strings starting with "#" may be comments, the same as with PBM.
//

int spectrogram_write_pgm(spectrogram_t *psd, char *filename)
{
	if(psd->magnitude == NULL) {
		log_printf( "spectrogram_write_pgm: Error writing spectrogram\n", filename);
		return(0);
	}

	FILE *fp = fopen(filename, "w");
	if(fp == NULL) {
		log_printf( "spectrogram_write_pgm: Error openning %s\n", filename);
		return(0);
	}
	log_printf( "spectrogram_wriet_pgm: creating image file %s\n", filename);
	
	int n;
	int N = psd->num_time_bins;
	int M = psd->num_freq_bins;

	unsigned short *pixel = (unsigned short *)(psd->magnitude);
	int pixel_count = M*N; // total num pixels
	int pixel_size = 2;  // two bytes per pixel

	float dfreq = psd->dfreq;
   	float dtime = psd->dtime;

	unsigned short maxval = 0;
	for(n = 0; n < pixel_count; n++) {
		if( pixel[n] > maxval) {
			maxval = pixel[n];
		}
	}

	fprintf(fp, "P5\n");  // magic number
	fprintf(fp, "# WISPR Spectrogram: df=%.3f Hz, dt=%.6f sec\n", dfreq, dtime);  // comment line
	fprintf(fp, "%d %d\n", M, N);  // Width and Height
	fprintf(fp, "%d\n", maxval);  // Max pixel value

	// if maxval is 8 bits, overwrite the buffer with 8 bit pixels
	if(maxval <= 256) {
		unsigned char *buf8 = (unsigned char *)pixel;
		pixel_size = 1;
		for(n = 0; n < pixel_count; n++) {
			buf8[n] = (unsigned char)(pixel[n]);
		}
	}

	// write image raster
	fwrite(pixel, pixel_size, pixel_count, fp);
	
	fclose(fp);

	return(n);
}

//
// save spectrogram raw binary data to a file
//
int spectrogram_write(spectrogram_t *psd, char *filename)
{
	if(psd->magnitude == NULL) {
		log_printf( "spectrogram_write: Error writing spectrogram\n", filename);
		return(0);
	}

	FILE *fp = fopen(filename, "w");
	if(fp == NULL) {
		log_printf( "spectrogram_write: Error openning %s\n", filename);
		return(0);
	}
	log_printf( "spectrogram_write_pgm: creating file %s\n", filename);
		
	int N = psd->num_time_bins;
	int M = psd->num_freq_bins;
	float dfreq = psd->dfreq;
   	float dtime = psd->dtime;
	int nwrt = 0;

	nwrt += fwrite(&M, 1, sizeof(int), fp);
	nwrt += fwrite(&N, 1, sizeof(int), fp);
	nwrt += fwrite(&dtime, 1, sizeof(float), fp);
	nwrt += fwrite(&dfreq, 1, sizeof(float), fp);
	nwrt += fwrite(psd->magnitude, 2, M*N, fp);

	fclose(fp);

	return(nwrt);
}

