/*
 * Wave_Test:  Continuously read data from a wave file.
 * Uses to overwrite a data buffer with know data from a wave. 
 * Useful for testing with a know data stream.
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
 * -------
 */

#include "wispr.h"

extern int verbose_level;  

static FILE *wave_fp = NULL;
static wav_file_header_t wave;
static off_t wave_off;
static size_t wave_nrd = 0;
static int wave_nsamps = 0;

//-----------------------------------------------------------------
int wav_test_init(char *wave_test_file, int nsamps, int fs)
{
   // open wave file and read header
   wave_fp = fopen(wave_test_file, "r");
   if(wave_fp == NULL) {
      log_printf("Error opening wave test file %s\n", wave_test_file);
	  return(-1);
   }

   // read wave file header
   // this sets the wave.nsamps to the number of samps in file
   wav_read_header(wave_fp, &wave);

   // check to make sure wave file can be used
   wave_nsamps = (int)(wave.Subchunk2Size)/(int)(wave.BitsPerSample/8); // num of samps
   if((wave.SampleRate != fs) || (wave_nsamps < nsamps)) {
      log_printf("wave_test_init: Invalid wave file data\n");
	  return(-1);	   
   }
   // can only handle 16 wave files for now
   if(wave.BitsPerSample != 16) {
      log_printf("wave_test_init: Error, only use 16 bit wave files\n");
	  return(-1);	   
   }
   
   if(verbose_level) {
	   log_printf("wave_test_init: %d samples, %d bit, %d Hz,\n", 
		  wave_nsamps, (int)wave.BitsPerSample, (int)wave.SampleRate);
   }

   // get file offset
   wave_off = ftell(wave_fp);

   wave_nrd = 0;

   return(1);
}

int wav_test_read(u_int32_t *buffer, int nsamps, int bitshift)
{
	if(wave_fp == NULL) {
		log_printf("wave_test_read: wave test file is not open.\n");
		return(-1);
	}
	if(buffer == NULL) {
		log_printf("wave_test_read: invalid buffer.\n");
		return(-1);
	}

	// check to make sure nsamps is not bigger than wav file size
	if(wave_nsamps < nsamps) {
		nsamps = wave_nsamps;
	}

	// read test data into the buffer.
	// since the same buffer is being used, the buffer is over written.
	// but the int16 words are read into the upper half of the int32 buffer
	int nrd = 0;
	int16_t *buf16 = (int16_t *)&buffer[nsamps/2];
	nrd = fread(buf16, 2, nsamps, wave_fp);	

	if(nrd != nsamps) {
		log_printf("wave_test_read: Error reading wave test file (%d).\n", nrd);
		return(nrd);
	}

	// update the wave nrd counter
	wave_nrd += nrd;

	// pack the buffer
	// bit shift in the opposite direction it to make it look like real adc data
	int n;
	for (n = 0; n < nsamps; n++) {
		buffer[n] = (u_int32_t)((int32_t)buf16[n] << bitshift);
	}

	if(verbose_level > 2) 
		log_printf("wave_test_read: Read total of %d samples from wave file\n", (int)wave_nrd);

    // check if there's enough left in file for another buffer read,
	// If not, then reset the file position and read from the start of the wave file data
	if(wave_nrd >= (wave_nsamps - nsamps)) { // Not enough for another full read
		fseek(wave_fp, wave_off, SEEK_SET);  // fseek back to start of wave file data
		if(verbose_level) log_printf("wave_test_read: file reset, nrd = %d\n", (int)wave_nrd);
		wave_nrd = 0;
	}

	return(nrd);
}

