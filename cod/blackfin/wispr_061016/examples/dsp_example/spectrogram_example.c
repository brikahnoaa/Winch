/*
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

char *progname;
int verbose_level = 0;

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
static int psd_fft_size = 256;		// fft size
static int psd_overlap = 128;		// overlap
static int psd_time_average = 4;    // number of time bins to average
static int psd_scaling_method = 1;  // see spectrogram.c

// ADC params
static sport_adc_t adc;                     // adc object
static int adc_fs = 125000;                 // sampling freq Hz
//static int adc_fs = 93750;                  // sampling freq Hz
static float adc_duration = 10.0;           // duration of each adc buffer (seconds)
static int adc_gain = 0;                    // pre amp gain 
static u_int32_t *active_adc_buffer = NULL; // pointer to current adc data buffer

// ad7766 read a 24bit word, but we want to save on 16 bits, so bit shift (>>) is 8
static int bitshift = 8;                    

// buffer counters
static int num_bufs_per_file = 10;         // max number of data buffers per file
static int buffer_count = 0;               // current number of buffer written to file

static int Led = 0;       // flag to enble/disable LEDs

//-----------------------------------------------------------------
// wave test mode 
// If wave_test_mode = 1 then the active_adc_buffer data will be overwritten
// with data from a wave file on each loop.
//
static int 	wave_test_mode = 0;  // enable/disable test mode
static char wave_test_file[32] = "/mnt/test.wav";

//-----------------------------------------------------------------
// local function prototypes
void make_filename(char *name, u_int32_t sec, char *prefix, char *ext);
void led_blinky();
void turn_on_preamp(int gain);
void turn_off_preamp();

void print_help()
{
	fprintf(stdout, "WISPR Data Logger Example \n");
	fprintf(stdout, "Options:           DESCRIPTION                                DEFAULT\n");
	fprintf(stdout, " -T {secs}         Size of ADC data buffers in seconds        [ 8 seconds ].\n");
	fprintf(stdout, " -n {fft size}     Size of FFT  						       [ 256 ]\n");
	fprintf(stdout, " -o {fft overlap}  FFT Overlap size  						   [ 128 ]\n");
	fprintf(stdout, " -a {num avg}      Number of time bins to average             [ 1 ]\n");
	fprintf(stdout, " -s {smeth}        FFT scaling method                         [ 1 ].\n");
	fprintf(stdout, " -C {number}       Max Number of data buffers to read         [ infinite ]\n");
	fprintf(stdout, " -v {level}        Verbose level (0=none)                     [ 0 ]\n");
	fprintf(stdout, " -W                Run in Wave Test Mode                      \n");
	fprintf(stdout, " -h                Print this help message\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " - Wave Test Mode:\n"); 
	fprintf(stdout, "           Test mode that reads data from a wave file and \n"); 
	fprintf(stdout, "           overwrites the adc buffer before processing the buffer.\n"); 
	fprintf(stdout, "\n");
}

/*
*  Main function call
*/
int main(int argc, char **argv)
{
	char *progname;
    char log_file[32]; 	// log file
	int usleep_time = 0;  // pause time between adc buffers in main loop

	progname = argv[0];

	// Get RAM size and check for pmem problems
	struct sysinfo info;
    sysinfo(&info);
	log_printf("Total Kernel RAM 0x%x\n", info.totalram);
	log_printf("BFIN_PMEM_START = 0x%x\n", BFIN_PMEM_START);
	if((BFIN_PMEM_START < info.totalram)) {
		log_printf("ERROR: BFIN_PMEM_START overlaps kernel memory.\n");
		log_printf("Make sure you compiled the code with the correct memory settings in wispr.h.\n");
		return(-1);
	}

	// mount data file system
	if(mount("/dev/sda1", "/mnt", "vfat", MS_REMOUNT, NULL) == 0) {
		log_printf("Mounted /mnt successful\n");
	}
	else if(errno == EBUSY) {
		log_printf("Mountpoint /mnt busy, mount is OK\n");
	}
	else {  // Error with sda1??
		log_printf("Mount sda1 - mnt error: %s\n", strerror(errno));
	}

	log_file[0] = 0;  // no log file, print to console

	int max_test_count = -1;  // used for testing only

	// parse command line args
	int opt;
	while ((opt = getopt(argc, argv, "T:o:s:n:a:C:b:p:l:v:LWh")) != EOF) {
		switch (opt) {
		case 'T': // adc buffer duration
			adc_duration = atof(optarg);
			break;
		case 's': // psd_scaling_method
			psd_scaling_method = atoi(optarg);
			break;
		case 'n': // fft size
			psd_fft_size = atoi(optarg);
			break;
		case 'o': // fft overlap
			psd_overlap = atoi(optarg);
			break;
		case 'a': // navg
			psd_time_average = atoi(optarg);
			break;
		case 'C': // number of files to record
			max_test_count = atoi(optarg);
			break;
		case 'W': // Enable wave test mode
			wave_test_mode = 1;
			//strncpy(wave_test_file, optarg, 32);
			break;
		case 'v':
			verbose_level = atoi(optarg);
			break;
		case 'h':
			print_help();
			return(0); // exit program
			break;
		}
	}

	// make sure the test can finish a complete file
	if((max_test_count > 0) && (max_test_count < num_bufs_per_file))  
		num_bufs_per_file = max_test_count;

	log_printf("WISPR PSD EXAMPLE STARTED\n");

	// open LEDs
	led_open(LED1);
	led_open(LED2);
	led_open(LED3);

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
			log_printf("Spectrogram: %f seconds to process\n",((double)(clock_stop-clock_start))/CLOCKS_PER_SEC);
			
			// save each spectrogram as a pgm image file
			char filename[64];
			make_filename(filename, sec, "psd", "pgm");
			spectrogram_write_pgm(&psd, filename);

			// used for testing only
			if((max_test_count > 0) && (test_count++ >= (max_test_count-1))) {
				go = 0;
			}

		}

		// slow things down
		if(usleep_time) usleep(usleep_time);

	}

wispr_close:

	// free spectrogram memory
	spectrogram_clear(&psd);

	turn_off_preamp();
	
	if(Led) {
		led_close(LED1);
		led_close(LED2);
		led_close(LED3);
	}

	// stop adc
	sport_adc_close(&adc);

	return 0;
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
