/*
 * WISPR Beaked whale detection and FLAC data logger with these modes:
 *
 * Mode 1: Record continuously, no detection functionality
 * Mode 2: Record continuously with detection functionality
 * Mode 3: Record intermittently.
 *         Skip specified number of buffers between files.
 * Mode 4: Run detection function only.
 *         Process incoming data continuously with the detection function,
 *         but only write a file when a detection appeared.
 *         The detection file has a different file prefix (set with dtx_file_prefix)
 * Mode 5: Run detection function and record data intermittently.
 *         Same idea as mode 4 + record data intermittently.
 *         Used to get an idea how many encounters were missed or
 *         to monitor noise levels regularly
 * Wave_Test_Mode:  Read data from a wave file instead of the ADC.
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
 * Embedded Ocean Systems (EOS), June 2015
 * -------
 * cjones 5/2014: 
 *   updated to use new libwispr and libwispr_flac libraries
 *   added free_disk_space function
 *   removed most calls to statfs, except at startup
 * cjones 5/2014:
 *   added clock cycle counting 
 * cjones 6/2015: upgraded to new libwispr and flac 1.3.1
 * aturpin 5/2016: uncommented umount.
 * aturpin 6/2016: updated for cuviers beaked whale including ici min/max.
 */

#include "wispr.h"
#include "wispr_flac.h"
#include "detect_bw.h"
#include "wav_test.h"

char *progname;
int verbose_level = 0;  
      
//-----------------------------------------------------------------
// bfin physical (non-kernel) memory region, defined in uboot using:

// setenv bootargs root=/dev/mtdblock0 rw clkin_hz=25000000 earlyprintk=serial,uart0,115200 mem=32M max_mem=64M$# console=ttyBF0,115200
//#define BFIN_PMEM_START 0x2000000 /* use when mem=32M */
//#define BFIN_PMEM_END 0x3F00000     /* use when max_mem=64M */

// setenv bootargs root=/dev/mtdblock0 rw clkin_hz=25000000 earlyprintk=serial,uart0,115200 mem=48M max_mem=64M$# console=ttyBF0,115200
#define BFIN_PMEM_START 0x3000000   /* use when mem=48M */
#define BFIN_PMEM_END 0x3F00000     /* use when max_mem=64M */

#define BFIN_PMEM_SIZE (BFIN_PMEM_END - BFIN_PMEM_START)
  
// If using non-kernel memory for adc buffers set ADC_USE_NONKERNEL_MEMORY=1
#define ADC_USE_NONKERNEL_MEMORY 1
  
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

static int wispr_mode = 1;  // default mode

// ADC params
static sport_adc_t adc;                // adc object
static int adc_fs = 125000;            // sampling freq Hz
static float adc_duration = 5.0;       // duration of each adc buffer (seconds)
static int adc_gain = 0;               // pre amp gain
static int bitshift = 8;               // bit shift
static u_int32_t *active_adc_buffer;   // pointer to adc buffer with the latest data
static int16_t *active_int16_buffer;   // pointer to latest int16 converted data buffer 

// flac data file params
static wispr_flac_t flac;   // flac object
static char flac_path[32] = "/mnt";      // location of data files
static char flac_prefix[16] = "wispr_";  // data file name prefix
static int flac_comp_level = 4;          // default flac compression level
static int flac_nbps = 16;               // default flac number of bits per sample

// COM port params
static com_t com;					// com message object
static char tty_device[32] = "/dev/ttyBF1";	// serial port device file
static int prev_com_state;

static char organization[32];
static char location[32];

// Watchdog timer
static int watchdog = 1;     // watchdog on
static int wdt_timeout = 20; // watchdog timer timeout

// buffer counters
static int num_bufs_per_file = 10;				// max number of data buffers per file
static int buffer_count = 0;					// current number of buffer written to file

// intermittent mode buffer counters
static int buffer_skip_count = 0;				// skipped buffer cound, used in mode 3 and 5
static int num_bufs_skipped_between_files = 5;	// used in modes 3 and 5

// Detection parameters
static int min_clicks_per_detection = 10;	// min number of clicks for a detection, see detect.h
static int num_bufs_per_detection = 5;		// number of adc bufs that define the detection duration
static float ici_thresh = 0.33;             // number of good clicks / total clicks
static float ici_min = 0.2;               // min ici seconds
static float ici_max = 0.6;                 //max ici seconds
static float thresh_factor = 170.0;

// Detection data file is used to save data buffer when a detection is found
static wispr_flac_t dtx_flac;                   // flac object
static char dtx_flac_path[32] = "/mnt";			// location of data files
static char dtx_flac_prefix[16] = "wispr_dtx_"; // data file name prefix
static int dtx_flac_comp_level = 2;				// default flac compression level
static int dtx_buffer_count = 0;				// current number of buffers written to dtx file

static int Led = 0;  // flag to enble/disable LEDs
static int Set_RTC = 0; // flag to set clock on startup via com port

//-----------------------------------------------------------------
// wave test mode
// If wave_test_mode = 1 then the active_adc_buffer data will be overwritten
// with data from a wave file on each loop.
//
static int 	wave_test_mode = 0;  // enable/disable test mode
static char wave_test_file[32] = "/mnt/bw_test.wav";

//-----------------------------------------------------------------
// local function prototypes

int process_com_messages(float timeout);
int process_mode_1();
int process_mode_2();
int process_mode_3();
int process_mode_4();
int process_mode_5();
int set_adc_gain(int gain);
void print_startup_banner();
float report_free_disk_space();
void print_free_disk_space();
float free_disk_space(float *free, float *total);
int exit_if_not_enough_disk_space(float min_percentage);

//-----------------------------------------------------------------
void print_help()
{
	fprintf(stdout, "WISPR Data Logger/Detector \n");
	fprintf(stdout, "Version X.X \n");
	fprintf(stdout, "Options:           DESCRIPTION                                DEFAULT\n");
	fprintf(stdout, " -M {mode}         Processing mode number                     [ 1 ]\n");
	fprintf(stdout, " -T {secs}         Size of ADC data buffers in seconds        [ 8 seconds ].\n");
	fprintf(stdout, " -F {level}        Sets flac compression level                [ 2 ].\n");
	fprintf(stdout, " -s {bitshift}     Sets data bitshift                         [ 8 ].\n");
	fprintf(stdout, " -o {organization} Organization string                        [ none ]\n");
	fprintf(stdout, " -b {number}       Number of data buffers per file            [ 10 ]\n");
	fprintf(stdout, " -n {nclick}       Min number of click for detection          [ 10 ]\n");
	fprintf(stdout, " -b {nbufs}        Number of ADC buffers for detection window [ 5 ]\n");
	fprintf(stdout, " -i {number}       Number buffers to skip between file        [ 10 ]\n");
	fprintf(stdout, " -p {prefix}       Data file name prefix                      [ wispr_ ]\n");
	fprintf(stdout, " -l {filename}     Log file name                              [ no log file ]\n");
	fprintf(stdout, " -v {level}        Verbose level (0=none)                     [ 0 ]\n");
	fprintf(stdout, " -L                Enable LEDs                                [ disabled ]\n");
	fprintf(stdout, " -r                Set RTC and lat/lon at startup             [ disabled ] \n");
	fprintf(stdout, " -W                Run in Wave Test Mode                      [ disabled ] \n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Modes: \n");
	fprintf(stdout, " - Mode 1: Record continuously, no detection functionality \n");
	fprintf(stdout, " - Mode 2: Record continuously with detection functionality \n");
	fprintf(stdout, " - Mode 3: Record intermittently. \n");
	fprintf(stdout, "           Skip specified number of buffers between files. \n");
	fprintf(stdout, " - Mode 4: Run detection function only. \n");
	fprintf(stdout, "           Process incoming data continuously with the detection function,\n");
	fprintf(stdout, "           but only write a file when a detection appeared.\n");
	fprintf(stdout, " - Mode 5: Run detection function and record data intermittently.\n");
	fprintf(stdout, "           Same idea as mode 4 + record data intermittently.\n");
	fprintf(stdout, "           Used to get an idea how many encounters were missed or \n");
	fprintf(stdout, "           to monitor noise levels regularly).\n");
	fprintf(stdout, "\n");
	fprintf(stdout, " - Wave Test Mode:\n");
	fprintf(stdout, "           Test mode that reads data from a wave file and \n");
	fprintf(stdout, "           overwrites the adc buffer before processing the buffer.\n");
	fprintf(stdout, "\n");
}

// cycle count timing  
static int do_clock_cycle_count = 0;  
static volatile clock_t clock_start;
static volatile clock_t clock_stop;
void print_clock_cycle_count(clock_t start, clock_t stop, char *label)
{
	if(do_clock_cycle_count)
	  printf("%f seconds for %s\n", ((double)(stop-start))/(double)CLOCKS_PER_SEC, label);
	  //printf("%08ld clock ticks for %s\n",(stop-start), label);
}
  
void print_startup_banner()
{
	log_printf("WISPR STARTED\n");
	
	// print disk space info
	float free, total, percent;
	percent = free_disk_space(&free, &total);
	log_printf("File system: %0.2f MB of %0.2f MB (%0.2f%%) available\n", 
		free/1000000.0, total/1000000.0, percent);
	
	unsigned long blksize = ADC_DATA_BLOCK_SIZE;
	int nsize = 4; // number bytes per sample, always 4
	int nsamps = (int)(adc_duration * (float)adc_fs);

	// number of data blocks in each adc buffer
	int nblks = (nsamps * nsize) / (int)blksize;

	// force the nsamps to be a multiple of blksize
	nsamps = (int)(nblks * blksize) / nsize;  // actual nsamps per buffer
	float duration = (float)nsamps / (float)adc_fs; // actual buffer duration

	float total_duration = (0.5*free)/(float)adc_fs;
	log_printf("Total recording time %0.2f hours\n", total_duration/(3600.0));

	log_printf("ADC buffer duration = %f sec, %d samples\n", duration, nsamps);
	log_printf("ADC fs = %d Hz, gain = %d, bitshift = %d\n", adc_fs, adc_gain, bitshift);
	
}
	
//-----------------------------------------------------------------
//
// Main loop, breaks when com.state == COM_EXIT
//
int main(int argc, char **argv)
{
    char log_file[32]; 	// log file
	int usleep_time = 0;  // pause time between adc buffers in main loop
	char msg_str[64];

	progname = argv[0];

	// mount data file system
	if(mount("/dev/sda1", "/mnt", "vfat", MS_REMOUNT, NULL) == 0) {
		fprintf(stdout, "Mounted /mnt successful\n");
	}
	else if(errno == EBUSY) {
		fprintf(stdout, "Mountpoint /mnt busy, mount is OK\n");
	}
	else {  // Error with sda1??
		fprintf(stdout, "Mount /mnt/sda1 - mnt error: %s\n", strerror(errno));
		return(0);
	}

	log_file[0] = 0;  // no log file, print to console

	flac.encoder = NULL;  //
	dtx_flac.encoder = NULL;  //

	int max_test_count = -1;  // used for testing only

	// parse command line args
	int opt;
	while ((opt = getopt(argc, argv, "M:T:s:g:C:i:n:b:m:f:F:o:l:p:v:rLhWx")) != EOF) {
		switch (opt) {
		case 'M': // wispr mode
			wispr_mode = atoi(optarg);
			if(wispr_mode < 1 || wispr_mode > 6) wispr_mode = 1;
			break;
		case 'f': // adc sampling frequency
			adc_fs = atoi(optarg);
			break;
		case 'T': // adc buffer duration
			adc_duration = atof(optarg);
			break;
		case 's': // bit shift
			bitshift = atoi(optarg);
			break;
		case 'g': // gain
			adc_gain = atoi(optarg);
			break;
		case 'C': // number of files to record
			max_test_count = atoi(optarg);
			break;
		case 'i': //
			num_bufs_skipped_between_files = atoi(optarg);
			break;
		case 'b': //
			num_bufs_per_file = atoi(optarg);
			break;
		case 'l': // override default log file name
			strncpy(log_file, optarg, 32);
			break;
		case 'p': // output data file prefix
			strcpy(flac_prefix, optarg);
			break;
		case 'F': // flac compression level XXXXX
			flac_comp_level= atoi(optarg);
			if(flac_comp_level < 1 || flac_comp_level > 8) flac_comp_level = 2;
            dtx_flac_comp_level=flac_comp_level ;
			break;
		case 'o': // "ORGANIZATION" in flac metadata XXXXX
			strncpy(organization, optarg, 32);
			break;
		case 'n': // number of clicks per detection
			min_clicks_per_detection = atoi(optarg);
			break;
		case 'r': // set clock on startup
			Set_RTC = 1;
			break;
		case 'L': // LEDs enable
			Led = 1;
			break;
		case 'W': // Enable wave test mode
			wave_test_mode = 1;
			break;
		case 'v':
			verbose_level = atoi(optarg);
			break;
		case 'x':
			do_clock_cycle_count = 1;
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

	// if too little disk space then exit
	if(exit_if_not_enough_disk_space(0.1)) {
		return(0);
	}

	// print startup message
	print_startup_banner();

	if(Led) {
		if(verbose_level) fprintf(stdout, "LEDs will function\n");
		// open LEDs
		led_open(LED1);
		led_open(LED2);
		led_open(LED3);
	}

	// open AD7766 device
	if(sport_ad7766_open(&adc, adc_fs) < 0) {
		log_printf("Error opening sport0\n");
		return(0);
	}

	// set the number of dma buffers to 3
	sport_adc_set_number_of_dma_buffers(&adc, 3);

	// allocate adc dma buffers
	if(sport_adc_alloc_pmem_buffers(&adc, adc_duration, BFIN_PMEM_START, BFIN_PMEM_END) < 0) {
		log_printf("Error allocating adc dma buffers\n");
		goto wispr_close;
	}

	// open gpio lines for pre-amp control
	gpio_export(GPIO_SD); gpio_dir_out(GPIO_SD); // shutdown bit
	gpio_export(GPIO_G1); gpio_dir_out(GPIO_G1); // gain G1 bit
	gpio_export(GPIO_G0); gpio_dir_out(GPIO_G0); // gain G0 bit

	// turn pre-amp ON
	gpio_write(GPIO_SD, 1);

	// set pre-amp gain
	set_adc_gain(adc_gain);

	// open com port for communication with platform
	if(com_open (&com, tty_device, B9600) < 0) {
		//perror ("com_open");
		log_printf("Error opening tty device\n");
		goto wispr_close; // 4
	}
    sleep(1);
	
	// Read a serial message to set time & location
	if(Set_RTC) {
		if(verbose_level) log_printf("Requesting GPS message to set RTC.\n");
		int nrd = 0;
		com.gps.lat = 0.0; com.gps.lon = 0.0;
		com_write_msg(&com, "GPS"); //5/13/14 Sends $GPS* to MPC for request of GPS Time and Location
		nrd = process_com_messages(10.0);  // wait 10 secs for a responce
		if(nrd > 0) {
			sprintf(msg_str, "Received gps message from com.");
		} else {
			sprintf(msg_str, "No gps message from com.");
		}
	}
	else {
		sprintf(msg_str, "No RTC function, no clock or location set.");
	}

	sprintf(location, "lat=%f, lon=%f", com.gps.lat, com.gps.lon);
	if(verbose_level) log_printf("RTC message %s\n", msg_str);

	// Inquiry for updated Gain Values
	if(Set_RTC) {
		if(verbose_level) log_printf("Requesting NGN message.\n");
		com_write_msg(&com, "NGN");
		process_com_messages(10.0);  // wait 10 secs for a response
	}

	// open wave file for testing 
	if(wave_test_mode) {
		wav_test_init(wave_test_file, adc.nsamps, adc.fs);
	}

	// open watchdog
	if(wdt_timeout == 0) watchdog = 0;
	if(watchdog) wdt_open(wdt_timeout);

	// initialize click detection
	if(wispr_mode > 1) {
		if(init_detect(min_clicks_per_detection, adc.nsamps, num_bufs_per_detection,
					   ici_thresh, thresh_factor, ici_min, ici_max) <= 0) {
			log_printf("Error initializing click detection\n");
			goto wispr_close;
		}
	}

	// start AD7766 clocks
	if(sport_ad7766_start_clock(&adc) <= 0) {
		log_printf("Error starting adc clocks\n");
		goto wispr_close;
	}

	// start adc dma read cycle using data buffers
	if(sport_adc_start_dma(&adc) < 0) {
		log_printf("Error starting ad7766\n");
		sport_adc_close(&adc);
		goto wispr_close;
	}

	// initial com state and mode
	com.state = COM_RUN;
	prev_com_state = COM_RUN;
	com.mode = 0;

	// read first dma buffer and discard it
	u_int32_t *junk;
	while(sport_adc_read_dma(&adc, &junk)) {};

	// turn leds off
	if(Led) {
		led_set_off(LED1);
		led_set_off(LED2);
		led_set_off(LED3);
	}

	// used when testing
	int test_count = 0;

	// clock tic variables to measure cycle count
	clock_t adc_clock_start = clock();
	clock_t adc_clock_stop = adc_clock_start;
	
	// loop over buffers
	buffer_count = 0; // reset buffer count
	while(com.state != COM_EXIT) {

		// keep watchdog alive
		if(watchdog) wdt_keep_alive();

		// read and process com port messages
		// - this updates com.state and com.mode
		process_com_messages(0.0);  // no wait time

		// read adc dma data buffer (non-blocking)
		// if a dma buffer is ready, then nrd > 0 and
		// a non-zero active buffer pointer will be set
		int nrd_adc = sport_adc_read_dma(&adc, &active_adc_buffer);

		// if paused, continue to read adc, but don't do anything with the data
		if(com.state == COM_PAUSE) {
			sleep(1);
			continue;
		}

		// exit state
		if(com.state == COM_EXIT) break;

		// when a new data bufer is ready, process it,
		// make sure processing finishes before next buffer is done
		// blink led 3 to show activity
		if(nrd_adc > 0) {

			// count adc clock cycles 
			adc_clock_stop = clock();  // buffer done 
			print_clock_cycle_count(adc_clock_start, adc_clock_stop, "adc read cycle");
			adc_clock_start = adc_clock_stop;  
			
			int ret = 0;

		    // if wave test mode is enabled,
		    // overwrite the active_adc_buffer with data from the wave file.
			// only call this after active_adc_buffer has been set a valid pointer
		    if(wave_test_mode) {
			   ret = wav_test_read(active_adc_buffer, adc.nsamps, bitshift);
		    }

			// Reformat 24 bit data from the ad7766 into 16 bits by shifting bits
			active_int16_buffer = sport_ad7766_reformat_int16(active_adc_buffer, adc.nsamps, bitshift);

			// switch between processing modes
			switch (wispr_mode) {
				case 1:
					ret = process_mode_1();
					break;
				case 2:
					ret = process_mode_2();
					break;
				case 3:
					ret = process_mode_3();
					break;
				case 4:
					ret = process_mode_4();
					break;
				case 5:
					ret = process_mode_5();
					break;
				default:
					log_printf("Unknown processing mode %d\n", wispr_mode);
					break;
			}

			// used for testing only
			if((max_test_count > 0) && (++test_count == max_test_count)) {
				com.state = COM_EXIT;
			}

		}

		// slow things down
		if(usleep_time) usleep(usleep_time);

	}

wispr_close:

	// stop watchdog timer
	wdt_close();

	// delete flac file object
	wispr_flac_delete(&flac);

	// turn pre-amp FFO
	gpio_write(GPIO_SD, 0);
	gpio_unexport(GPIO_SD);
	gpio_unexport(GPIO_G1);
	gpio_unexport(GPIO_G0);

	// stop adc
	sport_adc_close(&adc);

    // Tell MPC WISPR is done.
    com_write_msg(&com, "FIN");

	// clean up the rest
	com_close(&com);
	log_close();

	if(Led) {
		led_close(LED1);
		led_close(LED2);
		led_close(LED3);
	}

	// unmount data file system
	int count = 0;
	int n=1;
	while((n != 0) && (count < 4)) {
        system("cd /");
        sleep(1);
        n = system("umount -l /mnt");
        fprintf(stdout,"/bin/umount /mnt returned: %d\n",n);
        sleep(1);
        count++;
	}

	return 0;
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
		nrd = com_read_msg (&com, msg);

		// parse msg, if available
		// blink led 1 to show com activity
		if (nrd > 0) {

			if(Led) led_set_on(LED2);

			// parse custom messages like:
			// DX? - detection request 
			if (strncmp (msg, "DX?", 3) == 0) {
				int max_ndtx;
				sscanf (msg, "DX?,%d", &max_ndtx);
				com_send_dtx (&com, max_ndtx);
			}

			// parse standard messages
			com_parse_msg (&com, msg, nrd);

			if(com.mode == COM_GPS)  { //com->mode |= COM_GPS;
				sprintf(location, "lat=%f, lon=%f", com.gps.lat, com.gps.lon);
			}
			else if(com.mode == COM_GAIN)  {
				adc.gain = com.gain;
				set_adc_gain(adc.gain);
			}

			else if(com.mode == COM_STAT)  { // send % disk free
				report_free_disk_space();
			}
			
			else if(com.mode == COM_DETECT)  {
				min_clicks_per_detection = com.detx; // set new detection parameter
			}

			// stop the adc, if running
			if((com.state == COM_PAUSE) & (prev_com_state == COM_RUN)) {
				sport_adc_pause_dma(&adc);
				log_printf("%s: Paused\n");
			}
			// start adc, if not already running
			if((com.state == COM_RUN) & (prev_com_state != COM_RUN)) {
				sport_adc_resume_dma(&adc);
				log_printf("%s: Run\n", progname);
			}
			
			//if(verbose_level > 1)
			//	log_printf("state=0x%x, mode=0x%x\n", com.state, com.mode);

			prev_com_state = com.state;
			com.mode = 0;

			if(Led) led_set_off(LED2);

		}
		
		if(nrd>0) break;
		
		// if no timeout and no messages
		if((wait_usecs == 0) && (nrd == 0)) break;

		// wait for message// from 300* input time
		if (count_usecs < wait_usecs) {
			usleep(1);
			count_usecs++;
		}

		// wait has timed out
		if (count_usecs >= wait_usecs) {
			go = 0;
		}

	}

	return nrd;
}

//-----------------------------------------------------------------------
// Different modes of processing
//
// Mode 1: Record continuously no detection functionality
//
//

// Initialize the flac encoder
int initialize_flac()
{
	if(wispr_flac_create(&flac, flac_path, flac_prefix) < 0) {
		log_printf( "Error initializing flac file\n");
		return(-1);
	}
	// set the flac data file parameters
	// these can be set at any time, but will only be applied when a wispr_flac_init_file is called
	wispr_flac_set_sample_rate(&flac, adc_fs);
	wispr_flac_set_channels(&flac, 1);
	wispr_flac_set_compression_level(&flac, flac_comp_level);
	wispr_flac_set_bits_per_sample(&flac, flac_nbps);
	u_int32_t total_nsamps = adc.nsamps * num_bufs_per_file;  // total nsamps in file
	wispr_flac_set_total_samples_estimate(&flac, total_nsamps);
	wispr_flac_set_description(&flac, location);  // use location as description
	if(verbose_level > 0) log_printf( "FLAC Initialized\n");

	return(0);
}

int process_mode_1()
{
	if(Led) led_set_on(LED3);

	clock_start = clock();	// use clock to measure 
	
	// create flac encoder object, if not already done
	if(flac.encoder == NULL) {
		initialize_flac();
	}

	u_int32_t sec = adc.time.tv_sec; // sec timestamp for start of buffer
	u_int32_t usec = adc.time.tv_nsec/1000;  // usec timestamp for start of buffer

	// if current file is full then initialize a new flac file
	// or if buffer count was reset outside of this function
	if((buffer_count >= num_bufs_per_file) || (buffer_count == 0)) {

		// CHECK disk usage
		exit_if_not_enough_disk_space(0.1);

		// finish existing open flac data file
		wispr_flac_finish(&flac);

		// initialize a new flac data file using the encoder
		// if there's an error, re-initialize the encoder 
		if(wispr_flac_init_file(&flac, sec, usec) < 0) {
			wispr_flac_delete(&flac);
			initialize_flac();
		};

		// write meta data into a separate ascii header file
		// this is an alternative to flac metadata
		//wispr_flac_create_header_file(&flac, sec, usec);

		buffer_count = 0; // reset the buffer counter

	}

	// write the buffer to the current open flac file
	int nwrt = wispr_flac_write(&flac, (char *)active_adc_buffer, adc.nsamps, flac_nbps);
	if(nwrt < 0) log_printf( "Error writing flac file\n");

	buffer_count++; // increment the buffer counter

	if(Led) led_set_off(LED3);

	clock_stop = clock();
	print_clock_cycle_count(clock_start, clock_stop, "flac write");

	return(nwrt);
}

//
// Mode 2: Record continuously with detection functionality
//
int process_mode_2()
{
	int status = 0;

	// mode1 record
	process_mode_1();

	clock_start = clock();

	// click detection time
	if(Led) led_set_on(LED1);
	double  t0 = (double)(adc.time.tv_sec) + (double)(adc.time.tv_nsec)*0.000000001;
	if(Led) led_set_off(LED1);

	// run detect on data buffer
	int ndtx = detect(active_int16_buffer, adc.nsamps, adc.fs, t0, &status);

	clock_stop = clock();
	print_clock_cycle_count(clock_start, clock_stop, "detection");

	return(ndtx);
}

//
// Mode 3: Record intermittently.
// Skip the specified number of buffers in-between files
//
int process_mode_3()
{
	int ret = 0;

	// skip count is < max skip count, then skip this buffer and do nothing
	if((buffer_skip_count < num_bufs_skipped_between_files)) {
		buffer_skip_count++;
		return(ret);
	}

	// else if skip count >= max skip count, then record using mode1
	if(buffer_skip_count >= num_bufs_skipped_between_files) {
		ret = process_mode_1();
	}

	// if the current file is full, then start skipping buffers again
	if(buffer_count == num_bufs_per_file) {
		buffer_skip_count = 0;
	}

	return(ret);
}

//
// Mode 4: Run detection function only.
// Screen the incoming data continuously with the detection function
// but only write a file when a detection appeared.
//
// This is implemented by always saving the data buffers in a file
// and deleting the file is no detection is found.
// If a detection is found then just leave the file alone.
// Another way to to do this would be to save all the detection buffers in memory
// and then write them to file is a detection is found.
// This would do less disk i/o, but there isn't enough memory.
//
int process_mode_4()
{
	int status = 0;

	// click detection time using timestamp for start of buffer
	time_t sec = adc.time.tv_sec;
	u_int32_t usec = adc.time.tv_nsec/1000;

	double  t0 = (double)(sec) + (double)(usec)*0.000000001;

	// run detect on data buffer
	if(Led) led_set_on(LED1);
	int ndtx = detect(active_int16_buffer, adc.nsamps, adc.fs, t0, &status);
	if(Led) led_set_off(LED1);

	if(verbose_level > 2) {
		log_printf("Detection status = %d, ndtx = %d\n", status, ndtx);
	}

	// create dtx flac encoder object, if not already done
	if(dtx_flac.encoder == NULL) {
		if(wispr_flac_create(&dtx_flac, dtx_flac_path, dtx_flac_prefix) < 0) {
			log_printf( "Error initializing dtx flac file\n");
			return(-1);
		}
		// set the flac data file parameters
		wispr_flac_set_sample_rate(&dtx_flac, adc_fs);
		wispr_flac_set_channels(&dtx_flac, 1);
		wispr_flac_set_compression_level(&dtx_flac, dtx_flac_comp_level);
		wispr_flac_set_bits_per_sample(&dtx_flac, flac_nbps);
		u_int32_t total_nsamps = adc.nsamps * num_bufs_per_detection;  // total nsamps in file
		wispr_flac_set_description(&dtx_flac, location);
		wispr_flac_set_total_samples_estimate(&dtx_flac, total_nsamps);
	}
  
	// save buffers to dtx flac data file
	if(status & DETECT_PROCESSING) {
		// if current file is full or if buffer counter was reset
		// then initialize a new flac file
		if((dtx_buffer_count >= num_bufs_per_detection) || (dtx_buffer_count == 0)) {
			// finish existing openned flac detection data file
			wispr_flac_finish(&dtx_flac);
			// initialize a new flac data file using the encoder
			wispr_flac_init_file(&dtx_flac, sec, usec);
			// reset the buffer counter
			dtx_buffer_count = 0;
		}

		// write the buffer to the current open flac file
		int nwrt = wispr_flac_write(&dtx_flac, (char *)active_adc_buffer, adc.nsamps, flac_nbps);
		if(nwrt < 0) log_printf( "Error writing flac file\n");

		dtx_buffer_count++; // increment the buffer counter
	}

	// if a detection failed for any reason, then discard the data file
	else if(status & DETECT_FAILED) {
		// rm the current data file
		char cmd_str[64];
		sprintf(cmd_str, "/bin/rm %s/%s.flac", dtx_flac.path, dtx_flac.name);
	    system(cmd_str);  // call system cmd to rm file
		dtx_buffer_count = 0;  // reset buffer count to start a new data file
		log_printf("Removing file %s/%s\n", dtx_flac.path, dtx_flac.name);
	}

	// if a detection is found then keep the data file
	else if(status & DETECT_PASSED) {
		// reset buffer count to start a new data file
		// the next time a buffer is processed a new file will be
		dtx_buffer_count = 0;
		if(verbose_level > 1) {
			log_printf("Detection saved in %s/%s.flac\n", dtx_flac.path, dtx_flac.name);
		}
	}

	return(status);
}

//
// Mode 5: Run detection function and record data intermittently.
//  Same idea as mode 4 + record data intermittently
//  (e.g., for getting an idea how many encounters were missed or to monitor noise levels regularly).
//
int process_mode_5()
{
	int ret = 0;

	// Record intermittently
	process_mode_3();

	// Mode 4 detection
	process_mode_4();

	return(ret);
}



int set_adc_gain(int gain)
{
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
	return(gain);
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

void print_free_disk_space()
{
	float free, total, percent;
	percent = free_disk_space(&free, &total);
	log_printf("File system: %0.2f MB of %0.2f MB (%0.2f%%) available\n", 
		free/1000000.0, total/1000000.0, percent);
}

// send a DFP message to report disk usage
float report_free_disk_space()
{	
    char msg[COM_MESSAGE_SIZE];
	float free, total;
	float fs = free_disk_space(&free, &total);
	// send com DFP message
	sprintf(msg, "DFP,%.2f", fs);
    com_write_msg (&com, msg);	
	return(fs);
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
	

