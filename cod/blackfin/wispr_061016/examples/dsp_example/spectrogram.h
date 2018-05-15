/*
 * Spectrogram header file
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
 *
 */

#ifndef _SPECTROGRAM_H
#define _SPECTROGRAM_H

#include <math_bf.h>
#include <fract.h>
#include <filter.h>
#include <complex.h>
#include <window.h>

#define MAX_FFT_SIZE 4096 
#define MAX_FFT_POW2 12

struct spectrogram_struct {

	int nfft;      // size of fft
	int overlap;   // data overlap for each fft
	int navg;      // num of time bins to average
	int fs;        // sampling freq [hz]

	int scaling_method;     // 1 static, 2 dynamic, 3 none
	complex_fract16 *twiddle_table; // fft twiddle array
	complex_fract16 *obuf;  // fft input/output buffer array
	fract16 *ibuf;  // fft input/output buffer array
	fract16 *window;  // fft window

	float dtime;        // size of time bin in seconds.
	float dfreq;        // size of frequency bins in Hz.
	fract16 *magnitude; // fft buffer array
	int num_time_bins;  // ((nsamps - nfft)/(nfft - overlap)) / navg
	int num_freq_bins;  // nfft/2 +1

};
typedef struct spectrogram_struct spectrogram_t;

extern int spectrogram_init(spectrogram_t *psd, int fft_size, int overlap, int fs);
extern int spectrogram(spectrogram_t *psd, int16_t *input,  int nsamps, int navg, int scaling_method);
extern void spectrogram_clear(spectrogram_t *psd);
extern int spectrogram_write_pgm(spectrogram_t *psd, char *filename);
extern int spectrogram_write(spectrogram_t *psd, char *filename);

#endif /* _PSD_H */

