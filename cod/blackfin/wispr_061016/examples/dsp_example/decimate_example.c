/*
 * WISPR decimation example
 * This example shows how to read and decimate ADC data using the Blackfin DSP library.
 * The decimated data buffers are then logged to a wave file.
 * If running in wave test mode (-W) the buffers are read from /mnt/test.wav.
 * See decimate.c for details on FIR and IIR decimation filters.
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
#include "decimate.h"
#include "wav_test.h"   
#include <getopt.h>

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
//  setenv bootargs root=/dev/mtdblock0 rw clkin_hz=25000000 earlyprintk=serial,uart0,115200 
//     mem=48M max_mem=64M$# console=ttyBF0,115200
// Use printenv to verify the bootargs.  
// Then you need to tell the program where the non-kernel memory starts and stops 
#define BFIN_PMEM_START 0x3000000   /* use when mem=48M */
#define BFIN_PMEM_END 0x3F00000     /* use when max_mem=64M */
#define BFIN_PMEM_SIZE (BFIN_PMEM_END - BFIN_PMEM_START)
#define ADC_USE_NONKERNEL_MEMORY 1
// Be careful to make sure that the kernel and your program are consistent.

//---------------------------------------------------------------
// GPIO ADC Control lines
#define GPIO_SD GPIO_PG0
#define GPIO_G1 GPIO_PG1
#define GPIO_G0 GPIO_PG2

//---------------------------------------------------------------
// LED GPIO Lines
// Note that the LED GPIO lines have changed from V1.0 to V1.1

// if version 1.0
//#define LED1 GPIO_PG6
//#define LED2 GPIO_PG9
//#define LED3 GPIO_PG3

// if version 1.1
#define LED1 GPIO_PG6
#define LED2 GPIO_PG5
#define LED3 GPIO_PG4

//-----------------------------------------------------------------
//  Static user defined variables that control the processing 

// ADC parameters
static sport_adc_t adc;                     // adc object
static int adc_fs = 125000;                 // sampling freq Hz
static float adc_duration = 1.0;           // duration of each adc buffer (seconds)
static int adc_gain = 0;                    // pre amp gain 
static u_int32_t *active_adc_buffer = NULL; // pointer to current adc data buffer

static int decimation_type = 1;             // type = 1 for FIR, 2 for IIR
static int decimation_factor = 4;
static int new_fs;                 // decimated sampling freq Hz
static int new_nsamps;                      // number of samples after decimation

// ad7766 read a 24bit word, but we want to save on 16 bits, so bit shift (>>) is 8
static int bitshift = 8;                    
static int bits_per_sample = 16;         // save data as 16 bit words

// buffer counters
static int max_buffer_count = 10;
static int buffer_count = 0;               // current number of buffer written to file

static int Led = 0;       // flag to enble/disable LEDs

// wav data file parameters
static wav_file_t wav_file;                // data file object
static char wav_path[32] = "/mnt";         // location of data files
static char wav_prefix[16] = "wispr_";     // data file name prefix

//-----------------------------------------------------------------
// wave test mode 
// If wave_test_mode = 1 then the active_adc_buffer data will be overwritten
// with data from a wave file on each loop.
//
static int 	wave_test_mode = 0;  // enable/disable test mode
static char wave_test_file[32] = "/mnt/test.wav";

//-----------------------------------------------------------------
// local function prototypes
void led_blinky();
void turn_on_preamp(int gain);
void turn_off_preamp();

void print_help()
{
	fprintf(stdout, "WISPR Decimation Example \n");
	fprintf(stdout, "Options:           DESCRIPTION                                DEFAULT\n");
	fprintf(stdout, " -T {secs}         Size of ADC data buffers in seconds        [ 10 seconds ].\n");
	fprintf(stdout, " -R {factor}       Decimation factor (1,2,3,4,5)  			   [ 4 ]\n");
	fprintf(stdout, " -I                Use IIR decimation filter  			        \n");
	fprintf(stdout, " -F                Use FIR decimation filter  			        \n");
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
	//make_filename(log_file, "wispr_", 0, "log"); // make file_log name with date

	// parse command line args
	int opt;
	while ((opt = getopt(argc, argv, "T:R:C:M:v:Wh")) != EOF) {
		switch (opt) {
		case 'T': // adc buffer duration
			adc_duration = atof(optarg);
			break;
		case 'R': // decimation factor must = 1,2,3,4, or 5
			decimation_factor = atoi(optarg);
			break;
		case 'I': // decimation factor must = 1,2,3,4, or 5
			decimation_type = 2;
			break;
		case 'M': // decimation type (1 or 2)
			decimation_type = atoi(optarg);
			break;
		case 'C': // number of buffers to record
			max_buffer_count = atoi(optarg);
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

	// open ad7766 device
	if(sport_ad7766_open(&adc, adc_fs) < 0) {
		log_printf("Error openning sport ad7766\n");
		return(0);
	}

	// turn ON pre-amp and set gain
	turn_on_preamp(adc_gain);

	// allocate adc dma buffers
	if(sport_adc_alloc_pmem_buffers(&adc, adc_duration, BFIN_PMEM_START, BFIN_PMEM_END) < 0) {
		log_printf("Error allocating sport adc dma buffers\n");
		goto wispr_close;
	}

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

	// Initialize the wave data file
	new_fs = adc_fs / decimation_factor;
	if(wav_create(&wav_file, wav_path, wav_prefix) < 0) {
		log_printf( "Error initializing wav file\n");
		return(-1);
	}
	wav_set_sample_rate(&wav_file, new_fs);  // use new_fs instead of adc_fs
	wav_set_channels(&wav_file, 1);
	wav_set_bits_per_sample(&wav_file, bits_per_sample);
	wav_open(&wav_file, 0);

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
	int go = 1;
	while( go ) 
	{

		// read adc dma data buffer (non-blocking)
		// if a dma buffer is ready, then nrd > 0 and
		// a non-zero active buffer pointer will be set
		int nrd_adc = sport_adc_read_dma(&adc, &active_adc_buffer);

		// when a new data bufer is ready, process it,
		// make sure processing finishes before next buffer is done
		if(nrd_adc > 0) {

		    // if wave test mode is enabled, 
		    // overwrite the active_adc_buffer with data from the wave file.
			// only call this after active_adc_buffer has been set a valid pointer
		    if(wave_test_mode) {
			   wav_test_read(active_adc_buffer, adc.nsamps, bitshift);
		    }

			// reformat 32bit word from ad7766 into a 16 bit word 
			int16_t *buf16 = sport_ad7766_reformat_int16(active_adc_buffer, adc.nsamps, bitshift);

			// use clock to measure 
			clock_start = clock();

			// decimate the adc buffer
			float delay = 0.0;
			if(decimation_type == 2) {
				new_nsamps = decimate_iir_int16( buf16, adc.nsamps, decimation_factor, &delay );
			} else {
				new_nsamps = decimate_fir_int16( buf16, adc.nsamps, decimation_factor, &delay );
			}

			clock_stop = clock();
			double sec = (double)(clock_stop-clock_start)/CLOCKS_PER_SEC;
		
			if(decimation_type == 2) {
				log_printf("IIR Decimation: new buffer %d samples (%d Hz) processed in %f seconds\n", 
					new_nsamps, new_fs, sec);
			} else {
				log_printf("FIR Decimation: new buffer %d samples (%d Hz) processed in %f seconds\n", 
					new_nsamps, new_fs, sec);
			}
			
			// write the buffer to the current open file
			int nwrt = wav_write(&wav_file, (char *)buf16, new_nsamps);
			if(nwrt < 0) log_printf( "Error writing wav file\n");

			// exit if max buffer count
			if((max_buffer_count > 0) && (buffer_count++ >= (max_buffer_count-1))) {
				go = 0;
			}

		}
	}

wispr_close:
	
	if(Led) {
		led_close(LED1);
		led_close(LED2);
		led_close(LED3);
	}
	
	// turn OFF pre-amp
	turn_off_preamp();

	// stop adc
	sport_adc_close(&adc);

	return 0;
}

//----------------------------------------------------------------

void led_blinky()
{
	int m,n;
	for (m = 0; m < 10; m++) {
		for (n = 1; n <= 3; n++) {
			led_set_on(n);
			usleep(100000);
			led_set_off(n);
		}
	}
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
	gpio_unexport(GPIO_SD);
	gpio_unexport(GPIO_G1);
	gpio_unexport(GPIO_G0);
}

