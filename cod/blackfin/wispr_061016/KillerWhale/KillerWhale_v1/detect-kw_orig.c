/*
 * click_detect
 * Implementation of holger's beaked whale click detection
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
 * Embedded Ocean Systems (EOS), 2014
 * ---------------------------------------
 * Aug 2014: major revision to handle multiple buffer detections.
 * Sept 18, 2014: fixed problem with calculation of dtx_mean_low and ratio_thresh.
 *
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

//#include <cycle_count.h>

#include "detect.h"
#include "com.h"
#include "log.h"

#define SEND_DTX_MSGS

extern int verbose_level;


//-------------------------------------------------------------------------------
// filters using bfdsp functions
//

// IIR filter coefs defined by Holger for the OR data
/* Filter coeffs found in matlab as
fs = 93750;
[b1,a1]=butter(2, [43000 46000]/(fs/2));
fs = 93750;
[b2,a2]=butter(2, [5000 8000]/(fs/2));
*/

#define IIR_NSTAGES 2
float iir_b_high[2*IIR_NSTAGES+1] =
  {0.0088,         0,   -0.0177,         0,    0.0088};
float iir_a_high[2*IIR_NSTAGES] =
  {1,    3.6888,   5.1342,    3.1977,	0.7525};
float iir_b_low[2*IIR_NSTAGES+1] =
  {0.0088,         0,   -0.0177,         0,    0.0088};
float iir_a_low[2*IIR_NSTAGES] =
  {1,    -3.3871,   4.5981,    -2.9361,	0.7525};

// fmaxf is not defined in uclinux math lib, so this needs to be defined here
float fmaxf(float x, float y)
{
    if(x >= y) return(x);
    return(y);
}

void iir_filter(fract16 *input, fract16 *output, int nsamples,
   float *a_coeffs, float *b_coeffs, float gain)
{
   int i;
   float a[(2 * IIR_NSTAGES)]; // local coeffs
   float b[(2 * IIR_NSTAGES) + 1]; // local coeffs
   fract16 df1_coeffs[(4 * IIR_NSTAGES) + 2]; /* Coeff for iirdf1_fr16 function */
   fract16 delay[(4 * IIR_NSTAGES) + 2]; // delay line
   iirdf1_state_fr16 state; // filter state

   /* Initialize local coeffs and delay line */
   for (i = 0; i < (2*IIR_NSTAGES); i++) a[i] = a_coeffs[i];
   for (i = 0; i < (2*IIR_NSTAGES+1); i++) b[i] = gain * b_coeffs[i];
   for (i = 0; i < (4*IIR_NSTAGES+2); i++) delay[i] = 0;

   /* Convert coefficients */
   coeff_iirdf1_fr16 (a, b, df1_coeffs, IIR_NSTAGES);

   /* Initialize filter description */
   iirdf1_init (state, df1_coeffs, delay, IIR_NSTAGES);

   /* Call the function */
   iirdf1_fr16 (input, output, nsamples, &state);
}

//-------------------------------------------------------------------------------
/* FIR filter coefficients found in matlab
   fs = 93750;
   bw  = 3000;
   fc1 = 44500; % centre high
   fc2 = 6500; % centre low
   N = 16;
   w1 = [(fc1-bw/2) (fc1+bw/2)]/(fs/2);
   w2 = [(fc2-bw/2) (fc2+bw/2)]/(fs/2);
   h1 = fir1(N, w1, 'bandpass');
   h2 = fir1(N, w2, 'bandpass');
   % convert to int
   scale = 2^15;
   h11 = int16(fix(scale*h1)); % scale to fixed point
   h22 = int16(fix(scale*h2)); % scale to fixed point
*/
#define FIR_NCOEFFS 17

float fir_high[17] = {
   0.00311029131690047,	-0.00690678608099419,	0.0172780739912464,	-0.0361543798697327,
   0.0625997545418325,	-0.0927824774695729,	0.120917290358028,	-0.140902837271729,
   0.148134512877466,	-0.140902837271729,	0.120917290358028,	-0.0927824774695729,
   0.0625997545418325,	-0.0361543798697327,	0.0172780739912464,	-0.00690678608099419,
   0.00311029131690047
};
fract16 fir_high_f16[17] = {
    101,	-226,	566,	-1184,	2051,	-3040,	3962,	-4617,	4854,	-4617,	3962,
	-3040,	2051,	-1184,	566,	-226,	101
};
float fir_low[17] = {
	-0.0157323315509733,	-0.0245564971565786,	-0.0406837296914552,	-0.0464244577049938,
	-0.0209446968053998,	0.0428962981456339,	0.129039706167181,	0.203666059652644,
	0.233191705247532,	0.203666059652644,	0.129039706167181,	0.0428962981456339,
	-0.0209446968053998,	-0.0464244577049938,	-0.0406837296914552,	-0.0245564971565786,
};
fract16 fir_low_f16[17] = {
	-515,	-804,	-1333,	-1521,	-686,	1405,	4228,	6673,	7641,	6673,	4228,
	1405,	-686,	-1521,	-1333,	-804,	-515
};
void fir_filter(fract16 *input, fract16 *output, int nsamples,
   float *coeffs, float gain)
{
   int i;
   fir_state_fr16 state; // filter state
   fract16 d[FIR_NCOEFFS];
   fract16 h[FIR_NCOEFFS];
   for(i=0; i<FIR_NCOEFFS; i++) {
      d[i]=0;
      h[i] = float_to_fr16(gain*coeffs[i]);
   }
   fir_init(state, h, d, FIR_NCOEFFS, 1); // initialize filter
   fir_fr16(input, output, nsamples, &state);
}

//-------------------------------------------------------------------------------
// Initialize click detection
// This needs to be done before detect can be called
//
// The detection window size is hardwired in detect.h as
//  dtx.winsize = DETECTION_FILTER_WINDOW_SIZE.
//
// Detection filter window size (dtx.winsize) defines how many points to use in each data window.
// The input data buffer is processed in windows of this length.
// Therefore dtx.winsize should be an even factor of ADC_DATA_BLOCK_SIZE.
// Otherwise data may be missed by the detect() function.
// Look at detect() to understand how it's used before changing DETECTION_FILTER_WINDOW_SIZE.
//
// Number of detections windows (dtx.nwins) defines the total number of windows
// (each of size DETECTION_WINDOW_SIZE) that make up the detection duration.
// The value of dtx.nwins is defined by specifying the number of adc buffers to use
// for a detection (nbufs_per_detection).
// So dtx.nwins = (nsamps_per_buf / wdtx.windows) * nbufs_per_detection, where
// nsamps_per_buf is the size of the adc buffer.
// So the duration of the detection is dtx.winsize * dtx.nwins / fs,
// where fs is the sampling freq (Hz).
// For example, a 50 second detection window would use
// DETECTION_FILTER_WINDOW_SIZE = 1024, dtx.nwins = 6104, and with fs = 125000,
// then 1024*6104/125000 = 50 seconds.
//


static fract16 dtx_ibuf[DETECTION_FILTER_WINDOW_SIZE];  // filter input buffer
static fract16 dtx_obuf[DETECTION_FILTER_WINDOW_SIZE];  // filter output buffer
static float *dtx_ratio = NULL;		// filter ratio buffer
static int dtx_index;				// dtx_buffer index
static float dtx_mean_high;
static float dtx_mean_low;
static int dtx_initialized = 0;

static click_detect_t dtx;  // structure for detect parameters

int init_detect(int min_num_clicks, int nsamps_per_buf, int nbufs_per_detection, int bitshift)
{
	// check input args
   if(min_num_clicks <= 0) min_num_clicks = 1;
   if(bitshift < 0) bitshift = 0;
   if(bitshift > 8) bitshift = 8;
   if(nbufs_per_detection < 1) nbufs_per_detection = 1;

   /***User defined Parameters***/
   dtx.ici_thresh = 0.33; // number of good clicks / total clicks
   dtx.thresh_factor = 170.0;
   dtx.ici[0] = 0.005; // min ici seconds
   dtx.ici[1] = 0.15; //max ici seconds
   dtx.mode = 0;

   // wsize defines how many data point to use in each window
   // each data window is filtered and only the max value is saved
   int winsize = DETECTION_FILTER_WINDOW_SIZE; //(int)(0.010 * (float)fs);

   // check for valid numbers
   if (winsize > nsamps_per_buf) winsize = nsamps_per_buf;

   // force winsize to be an even multiple of buffer size
   winsize = nsamps_per_buf / (nsamps_per_buf / winsize);

   // nwins defines the total detect time window
   // so the duration of the detection is nwins*wsize/fs;
   int nwins_per_buf = (nsamps_per_buf / winsize);
   int total_nwins = nwins_per_buf * nbufs_per_detection;

   dtx.winsize = winsize;
   dtx.nwins = total_nwins;

   if(verbose_level > 2) log_printf("init_detect: winsize = %d, nwins = %d\n", dtx.winsize, dtx.nwins);

   // Sent from main, user parameter
   dtx.nclicks = min_num_clicks;
   dtx.bitshift = bitshift;

   // buffer to store the filtered time series
   if(dtx_ratio != NULL) free(dtx_ratio);  // free it if it exists

   // malloc a new dtx_ratio buffer
   dtx_ratio = (float *)malloc(sizeof(float)*total_nwins);
   if(dtx_ratio == NULL) {
      log_printf("init_click_detect: malloc error\n");
      return(-1);
   }

   // initialize the index counter.
   dtx_index = 0;
   dtx_mean_high = 0.0;
   dtx_mean_low = 0.0;
   dtx_initialized = 1;

    FILE *fp;
     fp = fopen (DTX_COM_FILE, "w");
  usleep(10000);
  fclose(fp);

   return(dtx.nwins);
}


//---------------------------------------------------------
// click detection
//
// This detection routine consists of two stages:
//  - a processing stage
//  - a detection stage.
//
// The function will process the input buffer each time it is called.
// However the detection stage will only be applied when enough processed data has accumulated.
// So it may take several calls and adc buffers to accumulate enough data windows to start the detection stage.
// This allows the adc buffer duration to be smaller than the total detection duration.
//
// The processing stage consists of:
//  - Looping over the adc buffer and copy a window of data of size dtx.winsize into a temporary buffer.
//  - Filter each data window with two filters (band pass filters centered around fc1 and fc2).
//  - Find the max value of each filtered data window and calculate the ratio if the max values
//    ratio = (fc2 filter max)/(fc1 filter max).
//  - Save the ratio in the global array dtx_ratio[].
//  - Increment the global array index (dtx_index).
//  - If the dtx_index is less than (dtx_index < dtx.nwins)
//    then return zero and set detect status to DETECT_PROCESSING.
//  - If enough detections windows has accumulated (that is dtx_index == dtx.nwins)
//    then continue on to the detction stage.
//
// The detection process consists of:
//  - Applying a teager-kaiser transform to dtx_ratio buffer.
//  - If the transformed ratio value is above a threshold, then save the time that the click occured
//    and count it as a potentially good click (increment the dtx_total_count).
//  - Then check if the click occured within a specified inter-click time interval (ici_min and ici_max)
//    of the previous click that also passed the threshold test.
//  - If the click passes the ici test, then count it as a good click (increment dtx_good_count).
//  - If after searching through the entire accumulated dtx_ratio buffer that there are too few good clicks,
//    then just return the number of good clicks found (dtx_good_count)
//    and set the return status to DETECT_FAILED & DETECT_FAILED_MIN_GOOD.
//  - If the ratio = dtx_good_count/dtx_total_count is less than a threshold (ratio < ici_thresh),
//    then again just return the number of good clicks found (dtx_good_count)
//    and set the return status to DETECT_FAILED & DETECT_FAILED_ICI.
//  - If there are enough good clicks and the ratio is greater than ici_thresh,
//    generate a detection message and save the message to file.
//  - Return the number of good clicks found (dtx_good_count)
//    and set the return status to DETECT_PASSED.
//
int detect(u_int32_t *buffer, int nsamps, int fs, double t0, int *status)
{
   int i, n, k;
   int ret_value = 0; // return value

   *status = 0;  // clear status flags

   const int wsize = dtx.winsize; //dtx.winsize;
   const int nwins_in_buffer = nsamps/wsize; // number of windows in data buffer
   const int shift = dtx.bitshift;

   if(verbose_level>2) log_printf("Starting Detections\n");

#ifdef _CYCLE_COUNTS
   cycle_t start_count;
   cycle_t final_count;
   START_CYCLE_COUNT(start_count);
#endif

   if(dtx_initialized == 0) {
      log_printf("detect: not initialized\n");
	  *status = DETECT_ERROR;
	  return(-1);
   }

   //---------------------------------------------------------------------------------------
   // loop over the input data buffer and filter it in blocks of size dtx.winsize.
   // filter the data windows with two filters (low and high bands).
   // calc ratio of the ration = (high max)/(low max) values in each filtered buffer.
   // Save the ration in the buffer dtx_ratio[dtx_index].
   //
   // n is the filter window nindex, k is data buffer index
   for(k = 0, n = 0; n < nwins_in_buffer; n++, k += wsize) {

      // copy a window of data into ibuf
	  // applying bitshift to scale the word into a fract16
      for(i=0; i<wsize; i++) {
         dtx_ibuf[i] = 0;
         if((i+k) < nsamps) dtx_ibuf[i] = (fract16)(buffer[i+k] >> shift);
      }

      // apply the high band filter to the window
      iir_filter(dtx_ibuf, dtx_obuf, wsize, iir_a_high, iir_b_high, 1.0);
      //fir_filter(dtx_ibuf, dtx_obuf, wsize, fir_high, 1.0);

      // find max of high band filtered data
      //max1 = vecmax_fr16(dtx_obuf, wsize);
	  fract16 max1 = 0;
      for(i=0; i<wsize; i++) {
         max1 = max_fr1x16(max1, dtx_obuf[i]);
      }

      // apply the low band filter to the window
      iir_filter(dtx_ibuf, dtx_obuf, wsize, iir_a_low, iir_b_low, 1.0);
      //fir_filter(dtx_ibuf, dtx_obuf, wsize, fir_low, 1.0);

      // find max of lower band filtered data
      //max2 = vecmax_fr16(dtx_obuf, wsize);
      fract16 max2=0;
	  for(i=0; i<wsize; i++) {
         max2 = max_fr1x16(max2, dtx_obuf[i]);
      }

      // check for zeros
      if(max1 == 0) max1 = 1;
      if(max2 == 0) max2 = 1;

      // calc ratio of max values
      float ratio = fr16_to_float(max1) / fr16_to_float(max2);
      if(ratio < 1.0) ratio = 0.0;
      else ratio = 20.0*log10f(ratio);

	  // store the means of the max
      dtx_mean_high += fr16_to_float(max1);
      dtx_mean_low += fr16_to_float(max2);

      // save result
      dtx_ratio[dtx_index] = ratio;

      // increment the ration buffer index
      dtx_index = dtx_index + 1;

	  // Check dtx index because it can grow too big in this loop.
	  // Note that if dtx.winsize is not an even fraction of the nsamps in adc buffer
	  // the end of the adc buffer may not get processed here.
	  if(dtx_index >= dtx.nwins)
		  break; // break for loop

   }

   if(verbose_level > 3) log_printf("detect: dtx ratio buffer index=%d\n",dtx_index);

   // check if the detection ratio buffer is full.
   // if it's not then exit, else proceed to the actual detection process.
   if(dtx_index < dtx.nwins) {
	   *status = DETECT_PROCESSING;
	   ret_value = 0;  // return zero detections
	   goto exit_click_detect;
   }

   //---------------------------------------------------------------------------------------
   // If it gets to this point the detection ratio buffer is full and ready to be processed
   //

   *status = 0;  // clear status flag

   //  NOTE, THIS IS HOW THE MEAN WAS CALCULATED IN PREVIOUS VERSIONS
   // but nwins_in_buffer is not the actual number of wins used in the average
//   dtx_mean_high = dtx_mean_high / (float)nwins_in_buffer;
//   dtx_mean_low = dtx_mean_low / (float)nwins_in_buffer;
   //  THIS IS HOW IT SHOULD BE FOUND
   dtx_mean_high = dtx_mean_high / (float)dtx.nwins;
   dtx_mean_low = dtx_mean_low / (float)dtx.nwins;

   // calc detection threshold
   float thresh_factor = dtx.thresh_factor;
   float ratio_thresh;
   if(dtx_mean_low == 0.0)  // special case if zero
      ratio_thresh = thresh_factor/120.0;  // is this a reasonable value???
   else
      ratio_thresh = thresh_factor/fabsf(log10f(fabsf(dtx_mean_low)));

   // now reset dtx_index and related variables
   dtx_index = 0;
   dtx_mean_high = 0.0;
   dtx_mean_low = 0.0;

   // initialize detection parameters
   float ici_min = dtx.ici[0]; // min number samples for ici
   float ici_max = dtx.ici[1]; // max number samples for ici
   float ici_thresh = dtx.ici_thresh;
   double dt = (double)wsize / (double)(fs);
   double prev_tc = 0.0;
   float mean_ratio = 0.0;
   float mean_tke = 0.0;
   float mean_ici = 0.0;

   if(verbose_level > 2) {
      log_printf("detect: mean_high=%.2f, mean_low=%.2f, ratio_thresh=%.2f\n",
         dtx_mean_high, dtx_mean_low, ratio_thresh);
   }

   // open file for testing only
   //FILE *test_fp = fopen("/mnt/dtx_ratio.txt","w");

   // now loop over all the windows in the full ratio buffer
   // to detect individual clicks
   int dtx_good_count = 0;
   int dtx_total_count = 0;
   int N = dtx.nwins;  // size of dtx_ratio[]
   for(n = 0; n < N; n++) {

      // apply teager-kaiser transform to ratio
      float tke;
      if(n==0)
         tke = dtx_ratio[n]*dtx_ratio[n] - dtx_ratio[n]*dtx_ratio[n+1];
      else if(n==(N-1))
         tke = dtx_ratio[n]*dtx_ratio[n] - dtx_ratio[n-1]*dtx_ratio[n];
      else
         tke = dtx_ratio[n]*dtx_ratio[n] - dtx_ratio[n-1]*dtx_ratio[n+1];

      // threshold the tke ratio
      if(tke >= ratio_thresh) {
         double tc = t0 + dt * (double)n; // click time stamp
         double ici = tc - prev_tc;
         prev_tc = tc;
         // it's a good click if the ici falls within a specified range
         if((ici <= ici_max) & (ici >= ici_min)) {
            dtx_good_count++;
            mean_ratio += dtx_ratio[n];
            mean_tke += tke;
            mean_ici += ici;
         }
         dtx_total_count++;
      }

      // dump data for testing
      //fprintf(test_fp,"%0.4f %0.4f\n", dtx_ratio[n], tke);

   }

   // close testing file
   //fclose(test_fp);

   // return number of detections
   ret_value = dtx_good_count;

   if(dtx_good_count > 1) {
      mean_tke /= (float)dtx_good_count;
      mean_tke /= (float)dtx_good_count;
      mean_ici /= (float)dtx_good_count;
   }

   // check for the miniumun number of good clicks
   // exit if not enough good clicks
   // and return the number of good click found in this buffer
   if(dtx_good_count < dtx.nclicks) {
	   *status |= DETECT_FAILED_MIN_GOOD;
   }

   // check ratio of good clicks to total
   // exit if ration is below the threshold
   // and return the number of good click found in this buffer
   if(((float)dtx_good_count / (float)dtx_total_count) < ici_thresh) {
	   *status |= DETECT_FAILED_ICI;
   }

   // if failed the tests
   if((*status & DETECT_FAILED_MIN_GOOD) || (*status & DETECT_FAILED_ICI)) {
	   *status |= DETECT_FAILED;
       goto exit_click_detect;
   }

   // else passed detection criteria
   *status = DETECT_PASSED;

   // make dtx message string
   FILE *fp;
   struct tm time;
   char tmstr[32];
   char dtx_str[128];
   time_t sec = (time_t)t0;  // start time second
   gmtime_r(&sec, &time);
   strftime(tmstr, sizeof(tmstr), "%d/%m/%g,%H:%M:%S", &time);
   sprintf(dtx_str,"DTX,%s,%d,%.3f,%.3f,%.3f,%.3f\n",
         tmstr, dtx_good_count, mean_tke, ratio_thresh, mean_ratio, mean_ici);

   if(verbose_level > 1) log_printf("%s", dtx_str);

   // open dtx log file and append detections
   // the log file is a backup of all the detection that stays on the system
   if((fp = fopen(DTX_LOG_FILE, "a+"))== NULL) {
      log_printf("detect: error openning dtx log file\n");
      *status |= DETECT_FAILED_LOG;
   }
   fprintf(fp,"%s", dtx_str);
   fclose(fp);

   // open dtx com file and append detections
   // The dtx com file is a detection queue that gets sent to the com port when requested.
   // After all dtx msgs have been sent, the dtx file is cleared, see function com_send_dtx.
   if((fp = fopen(DTX_COM_FILE, "a+"))== NULL) {
      log_printf("detect: error openning dtx com file\n");
      *status |= DETECT_FAILED_COM_LOG;
   }
   fprintf(fp,"%s", dtx_str);
   fclose(fp);

// exit point
exit_click_detect:

#ifdef _CYCLE_COUNTS
   STOP_CYCLE_COUNT(final_count,start_count);
   PRINT_CYCLES("detect: number of cycles = ",final_count);
#endif

   return(ret_value);
}

//---------------------------------------------------------
// send detection records
//  Open detection file, read dtxs, send each detection as a msg.
//  After all dtx msgs have been sent, the dtx file is cleared.
//  This way each dtx is only sent once.
int com_send_dtx (int ndtx_max)
{
  int n, ndtx, nclicks, m=0;
  char str[128], tmstr[20];
  FILE *fp;
  float mean_ici, mean_thresh, mean_ratio, mean_tke, mean_dur;
  float ici, thresh, ratio, tke, dur;
  struct timeval tv;
  struct tm time;
  char tty_device[32];

  strcpy(tty_device, "/dev/ttyBF1");
  // lock dtx file so no more detection can be written to it
  // until this func finishes
  n = ndtx = nclicks = 0;
  mean_ici = mean_dur = mean_thresh = 0.0;
  mean_ratio = mean_tke = 0.0;

  if((fp = fopen (DTX_COM_FILE, "r")) == NULL) {
    log_printf("com_send_dtx: error openning dtx com file\n");
  }

  // find number of click detects in file
  if(fp != NULL) {
    while (fgets (str, sizeof (str), fp) != NULL) {
      sscanf(str,"DTX,%17s,%d,%f,%f,%f,%f,%f",
        tmstr, &n, &tke, &thresh, &ratio, &ici, &dur);
      nclicks += n;
      mean_tke += tke;
      mean_thresh += thresh;
      mean_ratio += ratio;
      mean_ici += ici;
      mean_dur += dur;
      ndtx++;
    }
    fclose (fp);
  }

  if(ndtx > 0) {
    mean_tke /= (float)ndtx;
    mean_thresh /= (float)ndtx;
    mean_ratio /= (float)ndtx;
    mean_ici /= (float)ndtx;
    mean_dur /= (float)ndtx;
  }

  // create the DXN message
  // message time is the message creation time
  gettimeofday(&tv, NULL);
  gmtime_r((time_t *)&(tv.tv_sec), &time);
  strftime(tmstr, sizeof(tmstr), "%d/%m/%g,%H:%M:%S", &time);

  n = sprintf (str, "DXN,%04d,%s,%05d,%05d,%05d,%05d,%02d,%03d",
    ndtx, tmstr, nclicks, (int)mean_tke, (int)mean_thresh, (int)mean_ratio,
    (int)(mean_ici*100.0), (int)(mean_dur*10.0));

  //n = sprintf (str, "DXN,%05d,%s,%.3f,%.3f,%.3f,%.3f",
  //       ndtx, tmstr, mean_tke,mean_thresh, mean_ratio, mean_ici);

  // send DXN message to apf9
  com_write_msg (str, strlen(str));
  com_reset();

#ifdef SEND_DTX_MSGS

  // check if too many dtx, truncate if needed
  if (ndtx > ndtx_max) {
    ndtx = ndtx_max;
    if(verbose_level)
      log_printf("com_send_dtx: truncating number of DTX msgs\n");
  }
  if(verbose_level)
    log_printf("com_send_dtx: sending %d DTX records\n", ndtx);

  usleep(10000); // give the apf9 a little time

  // loop over ndtx and send each line in the detection file
  // open dtx file and send each detection
  fp = fopen (DTX_COM_FILE, "r");
  if(fp != NULL) {
   for (m = 0; m < ndtx; m++) {
    if (fgets (str, sizeof (str), fp) != NULL) {
      n = strlen (str);
      str[n-1] = 0;	// replace NL with null char
      n = strlen (str);
      // write msg to com, less the \n at the end
      com_write_msg (str, n);
      //if(verbose_level) log_printf("%d, %s\n", n, str);
      // wait for ack before sending next line

 /*     wait = 1;
      while(wait<200) { //Wait for ~2 msec or until ACK is received
         //usleep(10);
         com_read_msg(str, 10);
         if(strcmp(str,"ACK")==0) break;
         wait++;
      }*/
    }
   }
    com_reset();
    fclose(fp);

  }
usleep(10000);
#endif

  // clear the old data file, make a new empty one
  fp = fopen (DTX_COM_FILE, "w");
  usleep(10000);
  fclose(fp);
  usleep(10000);
  return (n);
}



