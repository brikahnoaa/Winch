/*
 * WISPR data logger example:
 * This example shows how to reads data continuously from the ADC 
 * then record the data buffers to files on the CF card as either FLAC 
 * or WAV formatted data files.
 * If compression_level > 0 then FLAC compression is used.
 * Otherwise data is logged in wave file format.
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
 * -------
 */

#include "wispr.h"
#include "wispr_flac.h"
#include "wav_test.h"   

char *progname;
int verbose_level = 0;  
int do_clock_cycle_count = 0;  
      
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
// LED GPIO Lines
// Note that the LED GPIO lines have changed from V1.0 to V1.1
// if version 1.0
#define LED1 GPIO_PG6
#define LED2 GPIO_PG9 // GPIO_PG5
#define LED3 GPIO_PG3 // GPIO_PG4

//-----------------------------------------------------------------
//  Static user defined variables that control processing

// ADC params
static sport_adc_t adc;          // adc object
static int adc_fs = 125000;              // sampling freq Hz
static float adc_duration = 5.0;        // duration of each adc buffer (seconds)
static int adc_gain = 0;                 // pre amp gain
static u_int32_t *active_adc_buffer;

// flac data file params
static wispr_flac_t flac;                // flac object
static char flac_path[32] = "/mnt";      // location of data files
static char flac_prefix[16] = "wispr_";  // data file name prefix
static int flac_comp_level = 4;          // default flac compression level

// desired number of bits per sample
static int bits_per_sample = 16;         // save data as 16 bit words
//static int bits_per_sample = 24;       // save data as 24 bit words

// bit shift to apply to ADC data word to get the desired nbps
static int bitshift = 8;   // bitshift = 8 for 16 bit data
//static int bitshift = 0;   // bitshift = 0 for 24 bit data

// COM port params
static com_t com;					        // com message object
static char tty_device[32] = "/dev/ttyBF1";	// serial port device file
static int prev_com_state;

// Watchdog timer
static int watchdog = 1;     // watchdog on
static int wdt_timeout = 20; // watchdog timer timeout

// buffer counters
static int num_bufs_per_file = 10;		// max number of data buffers per file
static int buffer_count = 0;			// current number of buffer written to file
static int max_test_count = -1;         // used for testing only

static char message_log_file[32]; 	// file to save print messages and output to stdout
static char description[] = "WISPR DATA LOGGER EXAMPLE";

static int Led = 0;  // flag to enble/disable LEDs

//-----------------------------------------------------------------
// wave test mode
// If wave_test_mode = 1 then the active_adc_buffer data will be overwritten
// with data from a wave file on each loop.
// 
static int 	wave_test_mode = 0;  // enable/disable test mode
static char wave_test_file[32] = "/mnt/test.wav";

//-----------------------------------------------------------------
// print cycle count timing  
static volatile clock_t clock_start;
static volatile clock_t clock_stop;
void print_clock_cycle_count(clock_t start, clock_t stop, char *label)
{
	if(do_clock_cycle_count)
	  printf("%f seconds for %s\n", ((double)(stop-start))/(double)CLOCKS_PER_SEC, label);
	  //printf("%08ld clock ticks for %s\n",(stop-start), label);
}

//-----------------------------------------------------------------
// local function prototypes
int record_flac();
int record_wave();
int process_com_messages(float timeout);
void hex_dump(u_int32_t *buf, int nsamps);
void led_blinky();
void print_free_disk_space();
float free_disk_space(float *free, float *total);
void turn_on_preamp(int gain);
void turn_off_preamp();
void print_startup_banner();

void print_help()
{
	fprintf(stdout, "WISPR Data Logger/Detector \n");
	fprintf(stdout, "Version X.X \n");
	fprintf(stdout, "Options:           DESCRIPTION                                DEFAULT\n");
	fprintf(stdout, " -T {secs}         Size of ADC data buffers in seconds        [ 8 seconds ].\n");
	fprintf(stdout, " -F {level}        Sets flac compression level                [ 2 ].\n");
	fprintf(stdout, " -r {nbps}         Number of bits per sample (8, 16, or 24)   [ 16 ].\n");
	fprintf(stdout, " -s {bitshift}     Sets data bitshift                         [ 8 ].\n");
	fprintf(stdout, " -b {number}       Number of data buffers per file            [ 10 ]\n");
	fprintf(stdout, " -p {prefix}       Data file name prefix                      [ wispr_ ]\n");
	fprintf(stdout, " -l {filename}     Message log file name                      [ no file ]\n");
	fprintf(stdout, " -v {level}        Verbose level (0=none)                     [ 0 ]\n");
	fprintf(stdout, " -L                Enable LEDs                                [ disabled ]\n");
	fprintf(stdout, " -W                Run in Wave Test Mode                      [ disabled ] \n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Modes: \n");
	fprintf(stdout, " - Wave Test Mode:\n");
	fprintf(stdout, "           Test mode that reads data from a wave file and \n");
	fprintf(stdout, "           overwrites the adc buffer before processing the buffer.\n");
	fprintf(stdout, "\n");
}

// parse command line args
void parse_command_line_args(int argc, char **argv)
{
	int opt;
	while ((opt = getopt(argc, argv, "M:T:r:s:g:C:i:n:b:m:f:F:o:l:p:v:rLhWx")) != EOF) {
		switch (opt) {
		case 'f': // adc buffer duration
			adc_fs = atoi(optarg);
			break;
		case 'T': // adc buffer duration
			adc_duration = atof(optarg);
			break;
		case 'r': // bit resolution
			bits_per_sample = atoi(optarg);
			if(bits_per_sample < 8) bits_per_sample = 8;
			if(bits_per_sample > 24) bits_per_sample = 24;
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
		case 'b': //
			num_bufs_per_file = atoi(optarg);
			break;
		case 'l': // override default log file name
			strncpy(message_log_file, optarg, 32);
			break;
		case 'p': // output data file prefix
			strcpy(flac_prefix, optarg);
			break;
		case 'F': // flac compression level XXXXX
			flac_comp_level= atoi(optarg);
			if(flac_comp_level < 0 || flac_comp_level > 8) flac_comp_level = 2;
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
			exit(0); // exit program
			break;
		}
	}
}

void print_startup_banner(sport_adc_t *adc)
{
	log_printf("WISPR STARTED\n");
	
	// print disk space info
	float free, total, percent;
	percent = free_disk_space(&free, &total);
	log_printf("File system: %0.2f MB of %0.2f MB (%0.2f%%) available\n", 
		free/1000000.0, total/1000000.0, percent);
	
	// force the nsamps to be a multiple of blksize
	float duration = (float)adc->nsamps / (float)adc->fs; // actual buffer duration

	float total_duration = (0.5*free)/(float)adc->fs;
	log_printf("Total recording time %0.2f hours\n", total_duration/(3600.0));

	log_printf("ADC buffer duration = %f sec, %d samples @ %d SPS\n", duration, adc->nsamps, adc->fs);
	log_printf("Logging %d bit words, shifted (>> %d)\n", bits_per_sample, bitshift);
}

/*
* Main function call
*/
int main(int argc, char **argv)
{
	int usleep_time = 0;  // pause time between adc buffers in main loop
	progname = argv[0];

	// mount the CD card file system to hold the data
	if(mount("/dev/sda1", "/mnt", "vfat", MS_REMOUNT, NULL) == 0) {
		fprintf(stdout, "Mounted /mnt successful\n");
	}
	else if(errno == EBUSY) {
		fprintf(stdout, "Mountpoint /mnt busy, mount is OK\n");
	}
	else {  // Error
		fprintf(stdout, "Mount /mnt/sda1 - mnt error: %s\n", strerror(errno));
		return(0);
	}
    
	// some defaults
	message_log_file[0] = 0;  // no log file, print to console
	flac.encoder = NULL;  // 

	// parse the inputs
	parse_command_line_args(argc, argv);
	
	// open log file for appending, otherwise log_printf will print to stdout
	// if one already exists then append new log messages to the end of it
	if(message_log_file[0] != 0) {
		log_open_append("/mnt", message_log_file);
	}

	// open LEDs
	led_open(LED3);
	led_open(LED2);
	led_open(LED1);
	led_blinky();

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

	// print startup message
	print_startup_banner(&adc);

	// turn ON pre-amp and set gain
	turn_on_preamp(adc_gain);

	// open com port for communication with platform
	if(com_open (&com, tty_device, B9600) < 0) {
		//perror ("com_open");
		log_printf("Error opening tty device\n");
		goto wispr_close;
	}
    sleep(1);

	// open watchdog
	if(wdt_timeout == 0) watchdog = 0;
	if(watchdog) wdt_open(wdt_timeout);

	// open wave file for testing 
	if(wave_test_mode) {
		wav_test_init(wave_test_file, adc.nsamps, adc.fs);
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
	while(1) {
		u_int32_t *junk;
		int nrd = sport_adc_read_dma(&adc, &junk);
		if(nrd > 0) {
			log_printf("First dma buffer ignored, nsamps = %d\n", nrd);
			break;
		}
		if(nrd < 0) log_printf("Error reading first dma buffer\n");
	}

	// turn leds off
	led_set_off(LED1);
	led_set_off(LED2);
	led_set_off(LED3);

	// used when testing
	int test_count = 0;

	// clock tic variables to measure cycle count
	clock_t adc_clock_start = clock();
	clock_t adc_clock_stop = adc_clock_start;
	
	// Loop and read ADC buffers
	// breaks when com.state = COM_EXIT,
	// or when the specified number of data buffers have been read
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
			// only call this after active_adc_buffer has been set to a valid pointer
		    if(wave_test_mode) {
			   ret = wav_test_read(active_adc_buffer, adc.nsamps, bitshift);
		    }
 
			//hex_dump(active_adc_buffer, 20);

			// Reformat 24 bit data from the ad7766 into desired word size
			// reformating overwrites the data buffer
			if( (bits_per_sample == 16) || (bits_per_sample == 8) )  {	
				// reformat to 16 bits by shifting bits
				sport_ad7766_reformat_int16(active_adc_buffer, adc.nsamps, bitshift);	
			}
			else if( bits_per_sample == 24 ) {
				if( flac_comp_level == 0 ) {	
					// reformat to 24 bits, if saving to wave file
					sport_ad7766_reformat_int24(active_adc_buffer, adc.nsamps);
				} else {
					// flac uses 32 bit input data for 24 bit output
					sport_ad7766_reformat_int32(active_adc_buffer, adc.nsamps);						
				}
			}
			else if( bits_per_sample == 32 ) {
				// reformat to 32 bits,
				sport_ad7766_reformat_int32(active_adc_buffer, adc.nsamps);	
			}
			else {
				log_printf("Unknown bits per sample %d\n", bits_per_sample);
			}
			
			// write the data to file 
			if( flac_comp_level > 0 ) {
				// using flac compression
				ret = record_flac();
			} else {
				// or a wave file with no compression
				ret = record_wave();
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

	// turn OFF pre-amp
	turn_off_preamp();

	// stop adc
	sport_adc_close(&adc);

    // Tell everyone that WISPR is done.
    com_write_msg(&com, "FIN");

	// clean up the rest 
	com_close(&com);
	log_close();

	if(Led) {
		led_close(LED1);
		led_close(LED2);
		led_close(LED3); 
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

			// parse the message
			com_parse_msg (&com, msg, nrd);

			if(Led) led_set_on(LED2);

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
			
			prev_com_state = com.state;
			com.mode = 0;

			if(Led) led_set_off(LED2);

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
  
//-----------------------------------------------------------------------
//  
// Record data to flac file
//
int record_flac()
{
	if(Led) led_set_on(LED3);

	clock_start = clock();	// use clock to measure 
	
	// create flac encoder object, if not already done
	if(flac.encoder == NULL) {
		if(wispr_flac_create(&flac, flac_path, flac_prefix) < 0) {
			log_printf( "Error initializing flac file\n");
			return(-1);
		}
		// set the flac data file parameters
		// these can be set at any time, but will only be applied when a wispr_flac_init_file is called
		wispr_flac_set_sample_rate(&flac, adc_fs);
		wispr_flac_set_channels(&flac, 1);
		wispr_flac_set_compression_level(&flac, flac_comp_level);
		wispr_flac_set_bits_per_sample(&flac, bits_per_sample);
		u_int32_t total_nsamps = adc.nsamps * num_bufs_per_file;  // total nsamps in file
		wispr_flac_set_total_samples_estimate(&flac, total_nsamps);
		wispr_flac_set_description(&flac, description);
		if(verbose_level > 0) log_printf( "FLAC Initialized\n");
	}

	u_int32_t sec = adc.time.tv_sec; // sec timestamp for start of buffer
	u_int32_t usec = adc.time.tv_nsec/1000;  // usec timestamp for start of buffer

	// if current file is full then initialize a new flac file
	// or if buffer count was reset outside of this function
	if((buffer_count >= num_bufs_per_file) || (buffer_count == 0)) {

		// finish existing open flac data file
		wispr_flac_finish(&flac);

		// initialize a new flac data file using the encoder
		// if there's an error, re-initialize the encoder 
		if(wispr_flac_init_file(&flac, sec, usec) < 0) {
			wispr_flac_delete(&flac);
		};

		// write meta data into a separate ascii header file
		// this is an alternative to flac metadata
		//wispr_flac_create_header_file(&flac, sec, usec);

		buffer_count = 0; // reset the buffer counter

	}  

	// write the buffer to the current open flac file
	int nwrt = wispr_flac_write(&flac, (char *)active_adc_buffer, adc.nsamps, bits_per_sample);
	if(nwrt < adc.nsamps) log_printf( "Error writing flac file\n");

	buffer_count++; // increment the buffer counter  

	if(Led) led_set_off(LED3);

	clock_stop = clock();
	print_clock_cycle_count(clock_start, clock_stop, "flac write");

	return(nwrt);
}

//-----------------------------------------------------------------------
// Record data to wav file
static wav_file_t wave;
static int wav_initialized = 0;

int record_wave()
{
	if(Led) led_set_on(LED3);

	u_int32_t sec = adc.time.tv_sec; // sec timestamp for start of buffer
	//u_int32_t usec = adc.time.tv_nsec/1000;  // usec timestamp for start of buffer

	// initialize raw data file, if first time 
	if(wav_initialized <= 0) {

		// Initialize the data file
		if(wav_create(&wave, flac_path, flac_prefix) < 0) {
			log_printf( "Error initializing wav file\n");
			return(-1);
		}
		wav_set_sample_rate(&wave, adc_fs);  // use new_fs instead of adc_fs
		wav_set_channels(&wave, 1);
		wav_set_bits_per_sample(&wave, bits_per_sample);  
		wav_initialized = 1;
	}
 
	// if current file is full then initialize a new file 
	// or if buffer count was reset outside of this function   
	if((buffer_count >= num_bufs_per_file) || (buffer_count == 0)) {

		// initialize a new data file using the encoder
		wav_open(&wave, sec);
   
		buffer_count = 0; // reset the buffer counter

	}

	// write the buffer to the current open file
	int nwrt = wav_write(&wave, (char *)active_adc_buffer, adc.nsamps);
	if(nwrt < 0) log_printf( "Error writing wav file\n");

	buffer_count++; // increment the buffer counter

	if(Led) led_set_off(LED3);
	
	return(nwrt);

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

//-----------------------------------------------------------------------
// dump the data to show how an ad7766 data word is formatted and manipulated
//
void hex_dump(u_int32_t *buf, int nsamps)
{
	int n;
	
	for (n = 0; n < nsamps; n++) {
		// ad7766 returns a 24 bit 2-comp word in a 32 bit uint32
		// turn it into a int32 by shifting left and right to preserve the sign bit 
		int32_t v1 = ((int32_t)(buf[n] << 8) >> 8);
		// now it has the correct sign when you apply right bitshift
		int32_t v2 = ((int32_t)(buf[n] << 8) >> (8 + bitshift));
		// now you cast it into an int16
		// but you need to check for overflow
		int16_t v3; // does not just = (int16_t)v2;
		if(v2 > 32767) v3 = 32767;
		else if (v2 < -32767) v3 = -32767;
		else v3 = (int16_t)v2;
		fprintf(stdout, "%d, %d, %d ", v1, v2, v3);
		fprintf(stdout, "(0x%08x, 0x%08x, 0x%08x)\n", v1, v2, v3);
	}
}

//-----------------------------------------------------------------------
// led blink to show activity
void led_blinky()
{
	int m;
	for (m = 0; m < 10; m++) {
		led_set_on(LED3);
		usleep(100000);
		led_set_off(LED3);
		led_set_on(LED2);
		usleep(100000);
		led_set_off(LED2);
		led_set_on(LED1);
		usleep(100000);
		led_set_off(LED1);
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
	
	// close GPIO lines
	gpio_unexport(GPIO_SD);
	gpio_unexport(GPIO_G1);
	gpio_unexport(GPIO_G0);
}

