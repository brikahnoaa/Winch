//Program to test the gain 5/03/2017 Haru Matsumoto
#include "wispr.h"
#include "wispr_flac.h"
#include "wav_test.h"   
char *progname;
int verbose_level = 0;  
int do_clock_cycle_count = 0;  

// GPIO ADC Control lines
#define GPIO_SD GPIO_PG0
#define GPIO_G1 GPIO_PG1
#define GPIO_G0 GPIO_PG2
#define BFIN_PMEM_END 0x3F00000     /* use when max_mem=64M */
#define BFIN_PMEM_SIZE (BFIN_PMEM_END - BFIN_PMEM_START)
#define ADC_USE_NONKERNEL_MEMORY 1
//---------------------------------------------------------------
// LED GPIO Lines
// Note that the LED GPIO lines have changed from V1.0 to V1.1
// if version 1.0
#define LED1 GPIO_PG6
#define LED2 GPIO_PG9 // GPIO_PG5
#define LED3 GPIO_PG3 // GPIO_PG4

static int adc_gain = 0;                 // pre amp gain
static int adc_fs = 125000;              // sampling freq Hz
static float adc_duration = 5.0;        // duration of each adc buffer (seconds)
static int bitshift = 8;   // bitshift = 8 for 16 bit data use this for power on duration HM
static int max_test_count = -1;         // used for testing only
static int bits_per_sample = 16;         // save data as 16 bit words

//buffer
static int num_bufs_per_file = 10;		// max number of data buffers per file

// COM port params
static com_t com;					     // com message object
static int Led = 0;  					 // flag to enble/disable LEDs

static char flac_prefix[16] = "wispr_";  // data file name prefix
static int flac_comp_level = 4;          // default flac compression level

static int 	wave_test_mode = 0;  // enable/disable test mode
static char message_log_file[32]; 	// file to save print messages and output to stdout

void turn_on_preamp(int gain);
void turn_off_preamp();
void parse_command_line_args(int argc, char **argv);
void led_blinky();
void parse_command_line_args(int argc, char **argv);
void print_help();
/*
* Main function call
*/
int main(int argc, char **argv)
{
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
    
	// parse the inputs
	parse_command_line_args(argc, argv);
	
	// print startup message
	log_printf("gaintest STARTED\n");

	// turn ON pre-amp and set gain
	turn_on_preamp(adc_gain);
	
	// open LEDs
	led_open(LED3);
	led_open(LED2);
	led_open(LED1);
	led_blinky();

    usleep(bitshift*1000000L); //1 = 8 sec

	// print startup message
	log_printf("Turn off pre-amp\n");
	
	// turn leds off
	led_set_off(LED1);
	led_set_off(LED2);
	led_set_off(LED3);

	// turn OFF pre-amp
	turn_off_preamp();

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
		case 's': // bit shift //use s to speify how long pre-amp is powered HM
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


void turn_on_preamp(int gain)
{
	// open gpio lines for pre-amp control
	gpio_export(GPIO_SD); gpio_dir_out(GPIO_SD); // shutdown bit
	gpio_export(GPIO_G1); gpio_dir_out(GPIO_G1); // gain G1 bit
	gpio_export(GPIO_G0); gpio_dir_out(GPIO_G0); // gain G0 bit

	// turn pre-amp ON
	gpio_write(GPIO_SD, 1);
	usleep(500000);  			//Wait for 500 ms HM 5/02/2017
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
//-----------------------------------------------------------------------
// led blink to show activity
void led_blinky()
{
	int m;
	for (m = 0; m < 10; m++) {
		led_set_on(LED3);
		usleep(100000); //original in usec
		led_set_off(LED3);
		led_set_on(LED2);
		usleep(100000);
		led_set_off(LED2);
		led_set_on(LED1);
		usleep(100000);
		led_set_off(LED1);
	}
}
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

