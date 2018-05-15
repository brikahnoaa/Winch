/*
 * Decimate using FIR and IIR filters
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
#ifndef _DECIMATE_H
#define _DECIMATE_H

#include <math_bf.h>
#include <fract.h>
#include <filter.h>
#include <fract2float_conv.h>

#define DECIMATE_FILTER_WINDOW_SIZE 1024

extern int decimate_iir_int16(int16_t *buffer, int nsamps, int R, float *delay);
extern int decimate_iir_int32(int32_t *buffer, int nsamps, int R, int shift, float *delay);
extern int decimate_fir_int16(int16_t *buffer, int nsamps, int R, float *delay);
extern int decimate_fir_int32(int32_t *buffer, int nsamps, int R, int shift, float *delay);

#endif

