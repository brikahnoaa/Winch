/*
 * Modified the sensitivity, HP of hydrophone and pre-amp gain ag 8 kHz. 
	9/20/2018 HM
 * Modified to estimate the 8-kHz noise level. This main and 
	spectrogram_write_pgm were modified. It computes the spectrogram and 
	average the spectral density over 1-kHz bandwidth.  Uses Nystuen's 
	eqn to convert the 8kHz noise to wind speed. 
	Haru Matsumoto June 6, 2017
 * WISPR spectrogram example:
 * This example shows how to builds a spectrogram from ADC data
 * using the Blackfin DSP library.  See spectrogram.c for details on
 * building spctrograms.
 * A spectrogram is built for each data buffer and saved as pgm image file.
 * If running in wave test mode (-W) the buffers are read from /mnt/test.wav.
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

#include "wispr.h"
#include "spectrogram.h"
#include <sys/sysinfo.h>
#include <time.h>
#include <com.h>
#include "log.h"
#include <math.h>

char *progname;
int  verbose_level = 1;

//-----------------------------------------------------------------
// Define the Physical (non-kernel) memory region used for the ADC data buffers. 
// This allows large contiguous memory regions for large data buffers.
// But since the kernel doesn't know about this memory, uboot must tell 
// the kernel how much memory there is in the system and how much the kernel will manage.  
// This is done by passing arguments to the kernel when it starts.  
// Here there is 64M of RAM and the kernel will only use 48M, leaving 16M for buffers.
// In this case, set the boot arguments in uboot as follows: 
//  setenv bootargs root=/dev/mtdblock0 rw clkin_hz=25000000 earlyprintk=serial,uart0,115200 mem=48M max_mem=64M$# console=ttyBF0,115200
// Use printenv to verify the bootargs.  
// Then you need to tell the program where the non-kernel memory starts and stops 
#define BFIN_PMEM_START 0x3000000   /* use when mem=48M */
#define BFIN_PMEM_END 0x3F00000     /* use when max_mem=64M */
#define BFIN_PMEM_SIZE (BFIN_PMEM_END - BFIN_PMEM_START)
#define ADC_USE_NONKERNEL_MEMORY 1
// Be careful to make sure that the kernel and your program are consistent.

//---------------------------------------------------------------
// LED GPIO Lines
// Note that the LED GPIO lines have changed from V1.0 to V1.1
#define LED1 GPIO_PG6
#define LED2 GPIO_PG5  // GPIO_PG9 for V1.0
#define LED3 GPIO_PG4  // GPIO_PG3 for V1.0

//-----------------------------------------------------------------
//  Static user defined variables that control the processing 

// Spectrogram parameters
static spectrogram_t psd;
static int psd_fft_size = 512;		// fft size
static int psd_overlap = 256;		// overlap
static int psd_time_average = 4;    // number of time bins to average
static int psd_scaling_method = 1;  // see spectrogram.c

 
// ADC params
static sport_adc_t adc;                     // adc object
static int adc_fs = 125000;                 // sampling freq Hz
//static int adc_fs = 93750;                // sampling freq Hz
static float adc_duration = 10.0;           // duration of each adc buffer (seconds)
static int adc_gain = 0;                    // pre amp gain 
static u_int32_t *active_adc_buffer = NULL; // pointer to current adc data buffer

// ad7766 read a 24bit word, but we want to save on 16 bits, so bit shift (>>) is 8
static int bitshift = 8;                    

// buffer counters
static int num_bufs_per_file = 10;         // max number of data buffers per file
static int buffer_count      = 0;          // current number of buffer written to file

static int led_light         = 0;      	 // flag to enble/disable LEDs
static int waittime          = 20;  	 	 // Time period to wait for CF2 inquirey

// COM port params
static com_t com;					             // com message object
static char tty_device[32] = "/dev/ttyBF1";// serial port device file
static int prev_com_state;

//-----------------------------------------------------------------
// wave test mode 
// If wave_test_mode = 1 then the active_adc_buffer data will be overwritten
// with data from a wave file on each loop.
//
static int 	wave_test_mode = 0;  // enable/disable test mode
static char wave_test_file[32] = "/mnt/test.wav";

//Noise level related 
int 	NL8kHz;
int 	WindSpeed;
int  	AveNL8kHz;

//-----------------------------------------------------------------
// local function prototypes
void make_filename(char *name, u_int32_t sec, char *prefix, char *ext);
//void led_blinky();
void turn_on_preamp(int gain);
void turn_off_preamp();
int exit_if_not_enough_disk_space(float min_percentage);
float free_disk_space(float *free, float *total);
int process_com_messages(float timeout);
int com_send_dtx (com_t *com, int ndtx_max);
void print_help();

/*
*  Main function call
*/
int main(int argc, char **argv)
{
	char *progname;
   char log_file[32]; 		 // log file
	int  usleep_time = 100;  // pause time between adc buffers in main loop
	int  AveNL    =  0;
	int  NormNL;
	int  NumM     =  0;
	int  hydr_sen = -175; //hydrophone sensitivity with 50 Hz HP HM 
	//int  PA_gain  =  37;  //pre-amp gain HM1_005
	int  PA_gain  =  47;  //pre-amp gain HM1 HM1506 modified 9/18/2018 HM
	int  fudge    =  30;  //fudge factor in dB to correct NL HM
	int  max_test_count = 10;  // number of files. default
	//int  write_pgm=  0;   //write *.pgm file

	progname = argv[0];

	log_file[0] = 0;  // no log file, print to console
	
	// Get RAM size and check for pmem problems
	struct sysinfo info;
    sysinfo(&info);
	
	// open com port for communication with platform
	if(com_open (&com, tty_device, B9600) < 0) {
		//perror ("com_open");
		fprintf(stdout, "Error opening tty device\n");
		goto wispr_close; // 4
	}
    sleep(1);
	
	log_printf("Total Kernel RAM 0x%x\n", info.totalram);
	log_printf("BFIN_PMEM_START = 0x%x\n", BFIN_PMEM_START);
	if((BFIN_PMEM_START < info.totalram)) {
		fprintf(stdout, "ERROR: BFIN_PMEM_START overlaps kernel memory.\n");
		fprintf(stdout, "Make sure you compiled the code with the correct memory settings in wispr.h.\n");
		return(-1);
	}
	
	// mount data file system
	if(mount("/dev/sda1", "/mnt", "vfat", MS_REMOUNT, NULL) == 0) {
		fprintf(stdout, "Mounted /mnt successful\n");
	}
	else if(errno == EBUSY) {
		fprintf(stdout, "Mountpoint /mnt busy, mount is OK\n");
	}
	else {  // Error with sda1??
		fprintf(stdout, "Mount sda1 - mnt error: %s\n", strerror(errno));
	}
	

	//int max_test_count = -1;  // used for testing only

	// parse command line args
	int opt;
	while ((opt = getopt(argc, argv, ":T:o:s:n:a:g:C:l:v:t:E:f:Wh")) != EOF){
		switch (opt) {
		case 'T': // adc buffer duration
			adc_duration = atof(optarg);
			break;
		case 'o': // fft overlap
			psd_overlap = atoi(optarg);
			break;
		case 's': // psd_scaling_method
			psd_scaling_method = atoi(optarg);
			break;
		case 'n': // fft size
			psd_fft_size = atoi(optarg);
			break;
		case 'a': // navg
			psd_time_average = atoi(optarg);
			break;
		case 'g': //gain
			adc_gain = atoi(optarg);
			break;
		case 'C': // number of files to record
			max_test_count = atoi(optarg);
			break;
		case 'l': // override default log file name
			strncpy(log_file, optarg, 32);
			break;			
		case 'v':
			verbose_level = atoi(optarg);
			break;
		case 't': // wait period for CF2 COM comm inquirey HM
			waittime = atoi(optarg);
			break;	
		case 'f':   //Added by HM
			fudge=atoi(optarg);
			break;
		case 'E':	//Added by HM
			led_light = atoi(optarg);
			break;	
		case 'W': // Enable wave test mode
			wave_test_mode = 1;
			//strncpy(wave_test_file, optarg, 32);
			break;
		case 'h':
			print_help();
			return(0); // exit program
			break;
		}
	}
	// open log file for appending, otherwise log_printf will print to stdout
	// if one already exists then append new log messages to the end of it
	if(log_file[0] != 0) {
		log_open_append("/mnt", log_file);
	}
	
	// if too little disk space then exit.  This takes a long time.
	
	//printf("Checking if disk has a enough space to write\n");	
	
	/*
	if(exit_if_not_enough_disk_space(0.0001)) {
		return(0);
	}
	*/

	// make sure the test can finish a complete file
	if((max_test_count > 0) && (max_test_count < num_bufs_per_file))  
		num_bufs_per_file = max_test_count;


	log_printf("WISPR SPECTROGRAM STARTED\n");
	log_printf("Fudge factor = %d dB\n", fudge);	


	// open LEDs
	if(led_light==1){
		log_printf("LED on when COM opens\n");
		led_open(LED1);
		led_open(LED2);
		//led_open(LED3);
		}
	
	// initialize spectrogram
	if(spectrogram_init(&psd, psd_fft_size, psd_overlap, adc_fs) < 0) {
		log_printf("Error initializing spectrogram\n");
		return(0);
	}

	// open ad7766 device
	if(sport_ad7766_open(&adc, adc_fs) < 0) {
		log_printf("Error openning sport ad7766\n");
		return(0);
	}

	// allocate adc dma buffers
	if(sport_adc_alloc_pmem_buffers(&adc, adc_duration, BFIN_PMEM_START, BFIN_PMEM_END) < 0) {
		log_printf("Error allocating sport adc dma buffers\n");
		goto wispr_close;
	}

	// turn ON pre-amp and set gain
	turn_on_preamp(adc_gain);

	// start ad7766 clocks
	if(sport_ad7766_start_clock(&adc) <= 0) {
		log_printf("Error starting ad7766 clocks\n");
		goto wispr_close;
	}

	// start adc dma read cycle using data buffers
	if(sport_adc_start_dma(&adc) < 0) {
		log_printf("Error starting sport adc\n");
		sport_adc_close(&adc);
		goto wispr_close;
	}

	buffer_count = 0;

	// initialize test mode wave file
	if(wave_test_mode) {
		wav_test_init(wave_test_file, adc.nsamps, adc.fs);
	}

	// read first dma buffer and discard it 
	while(1) {
		u_int32_t *junk;
		int nrd = sport_adc_read_dma(&adc, &junk);
		if(nrd > 0) {
			log_printf("First dma buffer ignored, nsamps = %d\n", nrd);
			break;
		}
		if(nrd < 0) log_printf("Error reading first dma buffer\n");
	} 

	volatile clock_t clock_start;
	volatile clock_t clock_stop;
	//printf("CLOCKS_PER_SECOND %ld\n",CLOCKS_PER_SEC);

	// loop over buffers
	int test_count = 0; 	// used used testing  
	int go = 1;
	buffer_count = 0; // reset buffer count
	float timeout;
	
	while(go) {

		// read adc dma data buffer (non-blocking)
		// if a dma buffer is ready, then nrd > 0 and
		// a non-zero active buffer pointer will be set
		int nrd_adc = sport_adc_read_dma(&adc, &active_adc_buffer);

		// when a new data bufer is ready, process it,
		// make sure processing finishes before next buffer is done
		if(nrd_adc > 0) {

			u_int32_t sec = adc.time.tv_sec; // sec timestamp for start of buffer

		    // if wave test mode is enabled, 
		    // overwrite the active_adc_buffer with data from the wave file.
			// only call this after active_adc_buffer has been set a valid pointer
		    if(wave_test_mode) {
			   if(wav_test_read(active_adc_buffer, adc.nsamps, bitshift) < 0) go = 0;
		    }

			// reformat 32bit word from ad7766 into a 16 bit word 
			int16_t *buf16 = sport_ad7766_reformat_int16(active_adc_buffer, adc.nsamps, bitshift);

			// use clock to measure 
			clock_start = clock();

			// build spectrogram
			spectrogram(&psd, buf16, adc.nsamps, psd_time_average, psd_scaling_method);
			
			clock_stop = clock();
			if(verbose_level >1)
			log_printf("Spectrogram: %f seconds to process\n",((double)(clock_stop-clock_start))/CLOCKS_PER_SEC);
   
   		if(led_light==1)led_set_on(LED1);
			
			// save each spectrogram as a pgm image file
			char filename[64];
			make_filename(filename, sec, "psd", "pgm");

			NL8kHz=spectrogram_write_pgm(&psd, filename);	 //HM Changed so that no file is created
			NL8kHz=NL8kHz-fudge*10;
			log_printf( "Uncalibrated 8 kHz NL %6.1f dB re V\n", (float)NL8kHz/10.);
			NormNL = NL8kHz-(hydr_sen+PA_gain+adc_gain*6)*10;//NL8kHz in pgm is ten times larger HM
			if(NumM>0)AveNL  = AveNL+NormNL;						 //HM First one is not reliable, Skip
			NumM=NumM+1;									 //HM
			log_printf("Acoustic NL %6.1f dB re uPa\n",(float)NormNL/10.);

			// used for testing only
			if((max_test_count > 0) && (test_count++ >= (max_test_count-1))) {
				go = 0;
			}
		}

		// slow things down
		if(usleep_time) usleep(usleep_time);
		if(led_light==1) led_set_off(LED1);

	}

wispr_close:
	//Average noise level 
	NumM=NumM-1;
	AveNL8kHz  =((int)(float)AveNL /(float)(NumM*10)); //devide by 10 here
	WindSpeed=(powf(10.,(float)AveNL8kHz/20.)+105.)/54.;//from J. Nystuen's paper. 16 is the fudge factor

	log_printf("Average noise level %d dB, Wind Speed %d m/s\n", AveNL8kHz, WindSpeed);
	
	
	timeout=(float)waittime;
	log_printf("Wait for %d sec CF2 inquiry $WS?*\n", waittime);
	
	com.state=COM_RUN;
	prev_com_state = COM_RUN;
	com.mode = 0;
	
   com_write_msg(&com, "RDY");
	process_com_messages(timeout);

	// free spectrogram memory
	spectrogram_clear(&psd);

	turn_off_preamp();
	
	if(led_light==1) {
		//led_close(LED1);
		led_close(LED2);
		//led_close(LED3);
	}
	// stop adc
	sport_adc_close(&adc);
	com_close(&com);
	log_close();

	// unmount data file system
	int count = 0;
	int n=1;
	while((n != 0) && (count < 4)) {
       system("cd /");
        sleep(1);
        n = system("umount -l /mnt");
        log_printf("/bin/umount /mnt returned: %d\n",n);
        sleep(1);
        count++;
	}	

	return 0;
}
void print_help()
{
	fprintf(stdout, "WISPR Data Logger Example \n");
	fprintf(stdout, "Options:           DESCRIPTION                                DEFAULT\n");
	fprintf(stdout, " -T {secs}         Size of ADC data buffers in seconds        [ 10 seconds ].\n");
	fprintf(stdout, " -n {fft size}     Size of FFT		    [ 256 ]\n");
	fprintf(stdout, " -o {fft overlap}  FFT Overlap size  	    [ 128 ]\n");
	fprintf(stdout, " -a {num avg}      Number of time bins to average             [ 1 ]\n");
	fprintf(stdout, " -s {smeth}        FFT scaling method                         [ 1 ].\n");
	fprintf(stdout, " -C {number}       Max Number of data buffers to read         [ 10 ]\n");
	fprintf(stdout, " -v {level}        Verbose level (0=none)                     [ 0 ]\n");
	fprintf(stdout, " -W                Run in Wave Test Mode                      \n");
	fprintf(stdout, " -g                pre-amp gain [0,1,2,3]\n");
	fprintf(stdout, " -t (secs)         wait time period to com with CF2\n");
	fprintf(stdout, " -l                log file name\n");
	fprintf(stdout, " -E (LED on)       LED light on\n");
	fprintf(stdout, " -f                fudge factor in dB to correct NL\n");
	fprintf(stdout, " -h                Print this help message\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " - Wave Test Mode:\n"); 
	fprintf(stdout, "           Test mode that reads data from a wave file and \n"); 
	fprintf(stdout, "           overwrites the adc buffer before processing the buffer.\n"); 
	fprintf(stdout, "\n");
}

//-----------------------------------------------------------------------
// Data processing - build a spectrogram for each buffer 
// and save it to an ascii file
//
void make_filename(char *name, u_int32_t sec, char *prefix, char *ext)
{
	// save each spectrogram to a text file
	char str[32];
	struct tm time;  
	time_t s = (time_t)sec;
	gmtime_r((time_t *)&s, &time);
	strftime(str, sizeof(str), "%g%m%d_%H%M%S", &time);
	sprintf(name, "/mnt/%s_%s.%s", prefix, str, ext);

}

//-----------------------------------------------------------------------
// HM1 Pre-amp control using GPIO 

// GPIO ADC Control lines
#define GPIO_SD GPIO_PG0
#define GPIO_G1 GPIO_PG1
#define GPIO_G0 GPIO_PG2

void turn_on_preamp(int gain)
{
	// open gpio lines for pre-amp control
	gpio_export(GPIO_SD); gpio_dir_out(GPIO_SD); // shutdown bit
	gpio_export(GPIO_G1); gpio_dir_out(GPIO_G1); // gain G1 bit
	gpio_export(GPIO_G0); gpio_dir_out(GPIO_G0); // gain G0 bit

	// turn pre-amp ON
	gpio_write(GPIO_SD, 1);

	// set gain
	if(gain == 0) {
		gpio_write(GPIO_G0, 0); // pre-amp G0=0
		gpio_write(GPIO_G1, 0); // pre-amp G1=0
	} else if(gain == 1) {
		gpio_write(GPIO_G0, 1); // pre-amp G0=0
		gpio_write(GPIO_G1, 0); // pre-amp G1=0
	} else if(gain == 2) {
		gpio_write(GPIO_G0, 0); // pre-amp G0=0
		gpio_write(GPIO_G1, 1); // pre-amp G1=0
	} else if(gain == 3) {
		gpio_write(GPIO_G0, 1); // pre-amp G0=0
		gpio_write(GPIO_G1, 1); // pre-amp G1=0
	}
}

void turn_off_preamp()
{
	// turn pre-amp OFF
	gpio_write(GPIO_SD, 0);
	
	// close GPIO lines
	gpio_unexport(GPIO_SD);
	gpio_unexport(GPIO_G1);
	gpio_unexport(GPIO_G0);
}
// set the exit flag if there's not enough disk space
// also return a non-zero if percentage free is too low
int exit_if_not_enough_disk_space(float min_percentage)
{
	// report disk usage
	float free, total;
	float percent = free_disk_space(&free, &total);
    if(percent < min_percentage){
        log_printf("Too little free space. Exiting\n");
        com_write_msg(&com, "FIN");
        com.state = COM_EXIT;
		return(1);
	}
	return(0);
}
//-----------------------------------------------------------------------
// Returns the number of bytes free in /mnt 
// and the percentage of the total available
//
float free_disk_space(float *free, float *total)
{	
    struct statfs stat;
	// check disk space
	if((statfs("/mnt", &stat)) < 0 ) {
		fprintf( stdout, "Failed to stat\n");
	}
	float blocks = (float)(stat.f_blocks); // Total data blocks in filesystem 
	//float bsize = 4096.0;  // hard code it
	// stat.f_bsize is the preferred size, not always the actual block size
	float bsize = (float)(stat.f_frsize); // Fragment size in bytes 
	float bfree = (float)(stat.f_bavail); // Free blocks available
	*free = bfree * bsize; 
	*total = blocks * bsize;
    float percent = 100.0 * (*free) / (*total);
	//printf("f_bsize (block size): %lu\n"
    //   "f_frsize (fragment size): %lu\n"
    //   "f_blocks (size of fs in f_frsize units): %lu\n"
    //   "f_bfree (free blocks): %lu\n"
    //   "f_bavail free blocks for unprivileged users): %lu\n",
    //   stat.f_bsize, stat.f_frsize, stat.f_blocks, stat.f_bfree, stat.f_bavail);
	return(percent);
}
//-----------------------------------------------------------------------
//
// Read and parse all com port messages received within timeout.
// If there are no com message, then it will loop until timeout is up.
// Timeout is the wait time in seconds.
// If timeout = 0, it returns without waiting.
// Update the com state and mode if a message if found in the com queue.
//
int process_com_messages(float timeout)
{
	char msg[COM_MSGSIZE];
	int nrd = 0;
	int wait_usecs = (int)(timeout * 300.0);  // usleep(1) seems to take ~300 usec
	int count_usecs = 0;
	
	int go = 1;

	while(go) {

		// check for com actions
		nrd = com_read_msg(&com, msg);
		// parse msg, if available
		// blink led 1 to show com activity
		if (nrd > 0) {

			if(led_light==1) led_set_on(LED2);

			// parse custom messages like:
			// WS? - wind speed request 
			if (strncmp (msg, "WS?", 3) == 0) {
				com_send_dtx (&com, nrd);
				return(nrd);
			}

			// parse standard messages
			//com_parse_msg (&com, msg, nrd);
		
			//if(verbose_level > 1)
			//	log_printf("state=0x%x, mode=0x%x\n", com.state, com.mode);

			prev_com_state = com.state;
			com.mode = 0;

			if(led_light==1) led_set_off(LED2);

		}

		// if no timeout and no messages
		if((wait_usecs == 0) && (nrd == 0)) break;

		// wait for message
		if (count_usecs < wait_usecs) {
			usleep(1);
			count_usecs++;
		}

		// wait has timed out
		if (count_usecs >= wait_usecs) {
			go = 0;
		}

	}

	return(nrd);
}

//---------------------------------------------------------
// send 8kHz noise and wind speed by serial

int com_send_dtx (com_t *com, int ndtx_max)
{
  int n;
  char str[COM_MESSAGE_SIZE], tmstr[20];
  struct timeval tv;
  struct tm time;
  char tty_device[32];

  strcpy(tty_device, "/dev/ttyBF1");

  /*if((fp = fopen (DTX_COM_FILE, "r")) == NULL) {
    log_printf("com_send_dtx: error openning dtx com file\n");
  }*/

  // create the message
  // message time is the message creation time
  gettimeofday(&tv, NULL);
  gmtime_r((time_t *)&(tv.tv_sec), &time);
  strftime(tmstr, sizeof(tmstr), "%m/%d/%g,%H:%M:%S", &time);

  n = sprintf (str, "NLW*,%s,%04d,%04d", tmstr, AveNL8kHz, WindSpeed);

  // send DXN message to CF2
  com_write_msg (com, str);

  if(verbose_level>1)
    log_printf("Com_send noise level and windspeed: sending %d %d\n", NL8kHz/10, WindSpeed);

  usleep(10000); // give the CF2 a little time
  // clear the old data file, make a new empty one
  return (n);
}
