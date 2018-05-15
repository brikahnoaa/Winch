/*
 * sport_adc.c - sport adc driver
 *
 * Uses the bfin_sport device driver, which uses dma descriptor lists
 * to write sport data into a circular buffer queue.
 * Uses the bfin_timer device driver to clock the adc.
 *
 * These sampling rates have been tested in ad7766_open.
 *   fs=125000, mclk=1000000, rsclk=5000000, sclk=125000000
 *   fs=93750, mclk=750000, rsclk=3750000, sclk=125000000
 *   fs=62500, mclk=500000, rsclk=2500000, sclk=125000000
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
 * 
 * Modifications:
 * - April, 2014: removed variable nsize, fixed at nsize = 4 
 * - April, 2014: removed malloc from adc_open
 * - May 2014, added pause and resume functions
 * - Jan 2015, changed ADC_BLOCK_SIZE from number of bytes to number of 32bit words
 * - Feb 2015, added driver for external ad7988 connected to sport1 on J6 (experimental)
 */

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <cycle_count.h>
#include <asm/bfin_sport.h>
#include <asm/bfin_timer.h>
#include <fract.h>
#include <sys/sysinfo.h>

#include "sport_adc.h"
#include "gpio.h"
#include "log.h"

extern int verbose_level;

#define GPIO_VREF GPIO_PG7

//#define EXT_RSCLK

//--------------------------------------------------------------------------
// AD7766 open and start functions
//
int sport_ad7766_open(sport_adc_t *adc, int fs)
{
   int fd, n;
   //adc_t *adc;

   struct sport_config config;

   if((fs != 125000) && (fs != 93750) && (fs != 62500)) {
      log_printf("ad7766_open: fs = %d not available, use 125k, 93.75k or 62.5k.\n", fs); 
      return(-1);
   } 

   // open "/dev/sport0" device
   fd = open("/dev/sport0", O_RDONLY, 0);
   if (fd < 0) {
      log_printf("ad7766: Failed to open /dev/sport0\n" );
      return(-1);
   }

   // sport recieve control
   memset(&config, 0, sizeof (struct sport_config));

   // use these settings with timer1 as rclk
   config.rfsr = 1;   // frame sync required
   config.larfs = 1;  // late frame sync
   config.irfs = 0;   // frame sync is generated externeally - by adc
   config.lrfs = 0;   // active high frame sync
   config.irclk = 0;  // serial clk generted external - by timer1
   config.rckfe = 0;  // sample data with rising edge of rclk
   config.word_len = 24;

   /* Configure sport controller by ioctl */  
   if (ioctl(fd, SPORT_IOC_CONFIG, &config) < 0) {
      log_printf("ad7766_open: failed to config sport\n");
      close(fd);
      return(-1);
   }

   // turn ADC VREF ON
   gpio_export(GPIO_VREF);
   gpio_dir_out(GPIO_VREF);
   gpio_write(GPIO_VREF, 0);
   usleep(1000);
   gpio_write(GPIO_VREF, 1);
   usleep(1000);

   // reset adc
   //gpio_adc_reset = gpio_open(GPIO_PG3, 'O');
   //gpio_write(gpio_adc_reset, '0');
   //gpio_write(gpio_adc_reset, '1');

   strncpy(adc->device, "/dev/sport0", 32);
   adc->fd = fd;
   adc->fs = fs;

   // set initial/default adc variables 
   adc->nbps = 24; // 24 bits of actual data in each word
   adc->nsize = 4; // always 4 bytes per sample in sport buffer
   adc->nbufs = 3; // number of dma buffers, should be > 2
   for (n=0; n<(adc->nbufs); n++) {
      adc->buffer[n] = NULL;
   }
   adc->gain = 0;
   adc->nchans = 1;  // not used
   adc->status = 0;

   return(1);
}

//
// Setup ADC clocks.  
// Timer0 is used for thet adc mclk. The serial bus uses timer1 as rsclk 
// Timer1 (rsclk) is synced to timer0 (mclk)
//
int sport_ad7766_start_clock(sport_adc_t *adc)
{
   unsigned long mclk, R, rsclk, sclk;
   unsigned long period, width;
   int fs;
   //float t4, t5;

   fs = adc->fs;
   R = 8;
   mclk = R*(unsigned long)fs;   // ad7766 fs = mclk/8

   // use a timer as the mclk signal
   adc->mclk_fd = open("/dev/timer0", O_RDONLY, 0);
   if (adc->mclk_fd < 0) {
      log_printf("ad7766_start_clock: Failed to open timer0\n");
      return(-1);
   } 

   // set mclk signal
   ioctl(adc->mclk_fd, BFIN_TIMER_OUTPUT_MODE);
   if (ioctl(adc->mclk_fd, BFIN_TIMER_SET_FREQ, &mclk) < 0) {
      log_printf("ad7766_start_clock: failed to set MCLK\n");
      return(-1);
   } 

   // get sclk 
   if (ioctl(adc->mclk_fd, BFIN_TIMER_GET_SCLK, &sclk)  < 0) {
      log_printf("ad7766_start_clock: failed to get SCLK\n");
      return(-1);
   } 

   // actual sampling freq
   //fs = mclk/R;  
   adc->fs = fs; 

   // rsclk is 5 times the mclk - found by testing
   rsclk = 5*mclk;  // rcv serial clk   
   //rsclk = 4000000;

   period = sclk/rsclk;
   width = period/2;

   // use a timer as the rsclk signal
   adc->rsclk_fd = open("/dev/timer1", O_RDONLY, 0);
   if (adc->rsclk_fd < 0) {
      log_printf("ad7766_start_clock: Failed to open timer1\n");
      return(-1);
   } 

   // set rsclk signal
   ioctl(adc->rsclk_fd, BFIN_TIMER_OUTPUT_MODE);
   ioctl(adc->rsclk_fd, BFIN_TIMER_SET_PERIOD, period);
   if (ioctl(adc->rsclk_fd, BFIN_TIMER_SET_WIDTH, width) < 0) { 
      log_printf("ad7766_start_clock: failed to set RSCLK\n");
      return(-1);
   }

   if(verbose_level > 1) {
      log_printf("ad7766_start_clock: fs=%d, mclk=%ld, rsclk=%ld, sclk=%ld\n", 
         fs, mclk, rsclk, sclk);
      if(verbose_level > 2) {
         log_printf("ad7766_start_clock: rsclk period=%ld, width=%ld\n", period,width);
      }
   }

   // start synchonized mclk and rsclk 
   ioctl(adc->mclk_fd, BFIN_TIMER_PULSE_HI);
   ioctl(adc->rsclk_fd, BFIN_TIMER_PULSE_HI);
   if (ioctl(adc->mclk_fd, BFIN_TIMER_START_SYNC, 1) < 0) {
      log_printf("ad7766_start_clock: failed to START\n");
      return(-1);
   } 

   if (ioctl(adc->mclk_fd, BFIN_TIMER_START) < 0) {
      log_printf("ad7766_start_clock: failed to START\n");
      return(-1);
   } 

   return(1);
}

//--------------------------------------------------------------------------
// AD7899 open and start functions
// uses /dev/sport1 
//
int sport_ad7988_open(sport_adc_t *adc, int fs)
{
   int fd, n;
   //adc_t *adc;
  
   struct sport_config config;

   if((fs < 0) || (fs > 500000)) {
      log_printf("ad7988_open: fs = %d not available.\n", fs); 
      return(-1);
   } 

   // open sport device
   fd = open("/dev/sport1", O_RDONLY, 0);
   if (fd < 0) {
      log_printf("ad7988: Failed to open /dev/sport1\n" );
      return(-1);
   }

   // sport recieve control
   memset(&config, 0, sizeof (struct sport_config));

   // recieve config control bits
   config.word_len = 16;
   config.rckfe = 1;  // clock falling edge select
   config.larfs = 0;  // late frame sync
   config.lrfs = 0;   // low frame sync select
   config.rfsr = 1;   // frame sync required select
   config.irfs = 1;   // internal frame sync select
   config.rlsbit = 0; // bit order, 0 for MSB first, 1 for LSB first
   config.irclk = 1;  // internal clock select
   config.rxsec = 0;  // Secondary side enabled

   // rclkdiv = (unsigned short) (sclk / (2 * config->rsclk) - 1);
   // rfsdiv = (unsigned short) (config->rsclk / config->rfsclk - 1);
   config.rfsclk = fs;   // recieve frame synce clk freq
   config.rsclk = 17*fs;    // recieve serial clk freq

   config.tckfe = 1;  // clock falling edge select
   config.latfs = 0;  // late frame sync
   config.ltfs = 0;   // low frame sync select
   config.tfsr = 1;   // frame sync required select
   config.itfs = 1;   // internal frame sync select
   config.tlsbit = 0; // bit order, 0 for MSB first, 1 for LSB first
   config.itclk = 1;  // internal clock select
   config.txsec = 0;  // Secondary side enabled
   //config.ditfs = 1;  // data indep frame sync select

   // rclkdiv = (unsigned short) (sclk / (2 * config->rsclk) - 1);
   // rfsdiv = (unsigned short) (config->rsclk / config->rfsclk - 1);
   config.tfsclk = fs;   // recieve frame synce clk freq
   config.tsclk = fs;    // recieve serial clk freq

   /* Configure sport controller by ioctl */  
   if (ioctl(fd, SPORT_IOC_CONFIG, &config) < 0) {
      log_printf("ad7988_open: failed to config sport\n");
      close(fd);
      return(-1);
   }

   strncpy(adc->device, "/dev/sport1", 32);
   adc->fd = fd;
   adc->fs = fs;

   // set initial/default adc variables 
   adc->nbps = 16; // 16 bits of actual data in each word
   adc->nsize = 4; // always 4 bytes per sample in sport buffer
   adc->nbufs = 3; // number of dma buffers, should be > 2
   for (n=0; n<(adc->nbufs); n++) {
      adc->buffer[n] = NULL;
   }
   adc->gain = 0;
   adc->nchans = 1;  // not used
   adc->status = 0;

   return(1);
}

//
// Setup ADC clocks.  
// Timer4 is used for the adc convert clk (mclk). 
// The serial bus uses timer3 as rsclk 
// Timer3 (rsclk) is synced to timer4 (mclk)
//
int sport_ad7988_start_clock(sport_adc_t *adc)
{
   unsigned long rfclk_period, rfclk_width, rfclk;
   unsigned long rsclk_period, rsclk_width, rsclk;
   unsigned long sclk, div, conv, cyc, fs;

   // use a timer as the CNV signal
   adc->mclk_fd = open("/dev/timer4", O_RDONLY, 0);
   if (adc->mclk_fd < 0) {
      log_printf("ad7988_start_clock: Failed to open timer5\n");
      return(-1);
   } 

   // get sclk 
   if (ioctl(adc->mclk_fd, BFIN_TIMER_GET_SCLK, &sclk)  < 0) {
      log_printf("ad7988_start_clock: failed to get SCLK\n");
      return(-1);
   } 

   ioctl(adc->mclk_fd, BFIN_TIMER_OUTPUT_MODE);

   // serial data clock
   fs =  (unsigned long)(adc->fs);
   cyc = sclk / fs;
   conv = (unsigned long)((float)sclk * 0.0000005);  // convert time
   div = 16 * cyc / (cyc - conv) + 1;
   fs =  sclk / cyc; 

   log_printf("ad7988_start_clock: fs=%ld, cyc=%ld, conv=%ld, div=%ld\n", fs, cyc, conv, div);

   rsclk = div * (unsigned long)(adc->fs);
   rsclk_period = sclk / rsclk;  
   rsclk_width = rsclk_period/ 2;
   rsclk = sclk / rsclk_period;  // actual rsclk

   // frame sync is the sampling clock
   rfclk = rsclk / div;
   rfclk_period = rsclk_period * div;
   rfclk_width = conv;

   if(verbose_level > 1) {
      log_printf("ad7988_start_clock: fs=%d, rfclk=%ld, rsclk=%ld, sclk=%ld\n", fs, rfclk, rsclk, sclk);
   }
   if(verbose_level > 2) {
      log_printf("ad7988_start_clock: rfclk period=%ld, width=%ld\n", rfclk_period, rfclk_width);
      log_printf("ad7988_start_clock: rsclk period=%ld, width=%ld\n", rsclk_period, rsclk_width);
   }

   if (ioctl(adc->mclk_fd, BFIN_TIMER_SET_PERIOD, rfclk_period) < 0) {
      log_printf("ad7988_start_clock: failed to set clock period\n");
      return(-1);
   } 
   if (ioctl(adc->mclk_fd, BFIN_TIMER_SET_WIDTH, rfclk_width) < 0) {
      log_printf("ad7988_start_clock: failed to set clock width\n");
      return(-1);
   }    

   // use a timer as the rsclk signal
   adc->rsclk_fd = open("/dev/timer3", O_RDONLY, 0);
   if (adc->rsclk_fd < 0) {
      log_printf("ad7988_start_clock: Failed to open timer1\n");
      return(-1);
   } 

   // set rsclk signal
   ioctl(adc->rsclk_fd, BFIN_TIMER_OUTPUT_MODE);
   if (ioctl(adc->rsclk_fd, BFIN_TIMER_SET_PERIOD, rsclk_period) < 0) { 
      log_printf("ad7988_start_clock: failed to set RSCLK period\n");
      return(-1);
   }
   if (ioctl(adc->rsclk_fd, BFIN_TIMER_SET_WIDTH, rsclk_width) < 0) { 
      log_printf("ad7988_start_clock: failed to set RSCLK width\n");
      return(-1);
   }
   
   // start synchonized rfclk and rsclk
   ioctl(adc->mclk_fd, BFIN_TIMER_PULSE_HI);
   ioctl(adc->rsclk_fd, BFIN_TIMER_PULSE_HI);
   if (ioctl(adc->rsclk_fd, BFIN_TIMER_START_SYNC, 4) < 0) {
      log_printf("ad7988_start_clock: failed to START SYNC\n");
      return(-1);
   } 

   return(1);
}


//
// set dma nbufs and check for valid arg
//
int sport_adc_set_number_of_dma_buffers(sport_adc_t *adc, int nbufs)
{
	if(nbufs <=0 || nbufs > 8) {
		log_printf("sport_adc_set_number_of_dma_buffers: FAILED, setting number of dma buffers to 3\n");
		nbufs = 3;
	}

	adc->nbufs = nbufs;
	return(nbufs);
}

/*
 * Allocate adc data buffers for dma data transfers from non-kernel memory (PMEM).
 * If the compiler directive ADC_USE_PMEM is defined, then a physical (non-kernel) memory region is used for the buffers.
 * If ADC_USE_PMEM is defined, then you need to define BFIN_PMEM_START as the start of the physical mem region.
 * Kernel memory configuration is set in the uboot bootargs.  
 * For example, adding the bootargs mem=32M max_mem=64M$# will tell the kernel to use 32M of the total 64M. 
 * This leaves 32M free as non-kernel mem.
 * Now use PMEM_START = 0x2000000 sets the physical mem start at 32M and PMEM_END = 0x4000000 to end at 64M. 
 * If PMEM_START is within the kernel memory, then the behavior is unpredictable.
 * 
 */
int sport_adc_alloc_pmem_buffers(sport_adc_t *adc, float adc_duration, unsigned long pmem_start, unsigned long pmem_end)
{
   size_t nbytes;
   unsigned long blksize;
   int nblks, n;
  
   // check to make sure nbufs is at least two
   if(adc->nbufs < 2) adc->nbufs = 2;

   blksize = ADC_DATA_BLOCK_SIZE;

   adc->nsize = 4; // number bytes per sample, always 4 because that's what dma uses
   adc->nsamps = (int)(adc_duration * (float)adc->fs);
   
   // number of data blocks in each adc buffer
   // nblks = (adc->nsamps * adc->nsize) / (int)blksize;  
   nblks = adc->nsamps / (int)blksize;   // mod Jan 2015

   // force the nsamps to be a multiple of blksize
   //adc->nsamps = (int)(nblks * blksize) / adc->nsize;  // actual nsamps per buffer
   adc->nsamps = (int)(nblks * blksize);  // actual nsamps per buffer, mod Jan 2015
   adc_duration = (float)adc->nsamps / (float)adc->fs; // actual buffer duration

   if(verbose_level > 1) {
      log_printf("sport_adc_alloc_pmem_buffers: adc buffer duration = %f\n", adc_duration);
      if(verbose_level > 2) 
         log_printf("sport_adc_alloc_pmem_buffers: adc nblks = %d, nsamps = %d\n", nblks, adc->nsamps);
   }

   // assign data buffers using malloc
   nbytes = 4*adc->nsamps; // always use 4 byte words for adc read buffer
    
   if(verbose_level > 1) {
	   log_printf("sport_adc_alloc_pmem_buffers: %ld bytes required for adc buffers\n", 
		   (nbytes * adc->nbufs));
   }
   
   // check RAM size
   struct sysinfo info;
   sysinfo(&info);
   if(pmem_start < info.totalram) {
		log_printf("sport_adc_alloc_pmem_buffers: ERROR: PMEM_START (0x%x) overlaps kernel (total RAM 0x%x)\n", 
		   pmem_start, info.totalram);
		return(-1);
   }
   if(verbose_level > 1) {
      log_printf("sport_adc_alloc_pmem_buffers: sysinfo total RAM 0x%x\n", info.totalram);
   }

   // assign data buffers using available phys mem
   void *pmem;  
   size_t poff, pinc;
   pmem = (void *)pmem_start; // non-kernel physical memory pointer
   poff = 0;
   pinc = (nbytes/4096+1)*4096; // increment on even page boundaries 
   for(n=0; n<(adc->nbufs); n++) {
      if(verbose_level > 2) 
		  log_printf("sport_adc_alloc_pmem_buffers: adc buffer[%d] = 0x%x - 0x%x\n", n, pmem+poff, pmem+poff+pinc-1);
	   adc->buffer[n] = (char *)(pmem+poff); // bit words
	   poff += pinc; // increment offset 
   }

   // set the tmp buffer
   //adc->tmp_buffer = (char *)(pmem+poff); // bit words
   //poff += (nbytes/4096+1)*4096; // increment on even page boundaries 
	   
   // check that the buffer are not over the size limit
   if((pmem+poff) >= (void *)(pmem_end)) {
		log_printf("sport_adc_alloc_pmem_buffers: ERROR: not enough physical memory for all the dama buffers\n");
		return(-1);
   }

   if(verbose_level > 1) {
      log_printf("sport_adc_alloc_pmem_buffers: BFIN_PMEM_START: 0x%x, BFIN_PMEM_END: 0x%x\n",
		  pmem_start, pmem_end);
   }

   // clear data buffers
   //for(n=0; n<(adc->nbufs); n++) {
   //   memset(adc->buffer[n], 0, nbytes);
   //}

   return(adc->nsamps);
}

/*
 * Allocate adc data buffers for dma and non-dma data transfers.
 * Buffers can be allocated from kernel managed memory (using malloc) or non-kernel memory.
 * If the compiler directive ADC_USE_PMEM is defined, then a physical (non-kernel) memory region is used for the buffers.
 * If ADC_USE_PMEM is defined, then you need to define BFIN_PMEM_START as the start of the physical mem region.
 * Kernel memory is set in the uboot bootargs.  
 * For example, adding the bootargs mem=32M max_mem=64M$# will tell the kernel to use 32M of the total 64M. 
 * This leaves 32M free as non-kernel mem.
 * Now use #define BFIN_PMEM_START 0x2000000 sets the physical mem start at 32M.
 * If BFIN_PMEM_START is within the kernel memory, then the behavior is unpredictable.
 * 
 */
int sport_adc_malloc_buffers(sport_adc_t *adc, float adc_duration)
{
   size_t nbytes;
   unsigned long blksize;
   int nblks, n;
  
   // check to make sure nbufs is at least two
   if(adc->nbufs < 2) adc->nbufs = 2;

   blksize = ADC_DATA_BLOCK_SIZE;

   adc->nsize = 4; // number bytes per sample, always 4 because that's what dma uses
   adc->nsamps = (int)(adc_duration * (float)adc->fs);
   
   // number of data blocks in each adc buffer
   // nblks = (adc->nsamps * adc->nsize) / (int)blksize;  
   nblks = adc->nsamps / (int)blksize;   // mod Jan 2015

   // force the nsamps to be a multiple of blksize
   //adc->nsamps = (int)(nblks * blksize) / adc->nsize;  // actual nsamps per buffer
   adc->nsamps = (int)(nblks * blksize);  // actual nsamps per buffer, mod Jan 2015
   adc_duration = (float)adc->nsamps / (float)adc->fs; // actual buffer duration

   if(verbose_level > 1) {
      log_printf("sport_adc_malloc_buffers: adc buffer duration = %f\n", adc_duration);
      if(verbose_level > 2) 
         log_printf("sport_adc_malloc_buffers: adc nblks = %d, nsamps = %d\n", nblks, adc->nsamps);
   }

   // assign data buffers using malloc
   nbytes = 4*adc->nsamps; // always use 4 byte words for adc read buffer
    
   if(verbose_level > 1) {
	   log_printf("sport_adc_malloc_buffers: %ld bytes required for adc buffers\n", 
		   (nbytes * adc->nbufs));
   }
   
   // assign data buffers with malloc using kernel memory
   for(n=0; n<(adc->nbufs); n++) {
	   adc->buffer[n] = (char *)malloc(nbytes); 
	   if(adc->buffer[n] == NULL) {
		   log_printf("Error allocating adc buffers\n"); 
		   return(-1);
	   }
   }

   if(verbose_level > 2) {
	   for(n=0; n<(adc->nbufs); n++) {
		   log_printf("sport_adc_malloc_buffers: adc buffer[%d] = %p\n", n, adc->buffer[n]);
	   }
   }


   // clear data buffers
   //for(n=0; n<(adc->nbufs); n++) {
   //   memset(adc->buffer[n], 0, nbytes);
   //}

   return(adc->nsamps);
}


int sport_adc_start_dma(sport_adc_t *adc)
{
   struct sport_dma_config dma_config;
   int n, bufsize, nbufs, xcount, ycount, word_len;
   
   bufsize = adc->nsamps;
   nbufs = adc->nbufs;
   word_len = 32;  // always 32 bits

   // find dma xcount and ycount
   xcount = 0xFFFF; 
   ycount = bufsize / xcount;
   while((xcount > 1) & ((xcount*ycount) != bufsize)) {
      xcount = xcount - 1;
      ycount = bufsize / xcount;
   }

   if(verbose_level > 2) {
     log_printf("sport_adc_start_dma: xcount = %d, ycount=%d\n", xcount, ycount);
   }

   // Configure dma buffers
   dma_config.mode = DMA_RX_QUEUE;  
   dma_config.word_len = (char)word_len;
   dma_config.row_size = (short)xcount;
   dma_config.num_buffers = nbufs;
   dma_config.buffer_size = bufsize;
   for (n=0; n<nbufs; n++) {
      if(adc->buffer[n] == NULL) {
         log_printf("sport_adc_start_dma: invalid buffer pointer.\n");
		 return(-1);
	  }
      dma_config.buffer[n] = (unsigned long)(adc->buffer[n]);
   }

   /* Configure sport dma controller by ioctl */
   if (ioctl(adc->fd, SPORT_DMA_CONFIG, &dma_config) < 0) {
      log_printf("sport_adc_start_dma: failed to config sport dma\n");
      return(-1);
   }

   /* Configure sport dma queue */
   if (ioctl(adc->fd, SPORT_DMA_START) < 0) {
      log_printf("sport_adc_start_dma: failed to start sport dma\n");
      return(-1);
   }

   return(1);

}

int sport_adc_pause_dma(sport_adc_t *adc)
{
   /* Stop sport dma */
   if (ioctl(adc->fd, SPORT_DMA_STOP) < 0) {
      log_printf("sport_adc_pause_dma: failed to stop sport dma\n");
      return(-1);
   }

   /* stop mclk */
   if (ioctl(adc->mclk_fd, BFIN_TIMER_STOP) < 0) {
      log_printf("sport_adc_pause_dma: failed to stop timer\n");
      return(-1);
   } 

   return(1);
}

int sport_adc_resume_dma(sport_adc_t *adc)
{
   /* start mclk */
   if (ioctl(adc->mclk_fd, BFIN_TIMER_START) < 0) {
      log_printf("sport_adc_resume_dma: failed to start timer\n");
      return(-1);
   } 

   /* Start sport dma */
   if (ioctl(adc->fd, SPORT_DMA_START) < 0) {
      log_printf("sport_adc_resume_dma: failed to start sport dma\n");
      return(-1);
   }

   return(1);
}


// 
// Read dma buffer if it's ready.
// If a buffer is ready to read, return the number of bytes.
// If dma buffer is not ready, return 0.
// The current dma buffer is set to adc->buffer.addr
// The dma buffer word is always 32 bits
//
int sport_adc_read_dma(sport_adc_t *adc, u_int32_t **buffer)
{
   int nsamps;
   struct timeval now;

   /* Read sport dma queue */
   if (ioctl(adc->fd, SPORT_DMA_READ, &(adc->dma_buffer)) < 0) {
      log_printf("sport_adc_read_dma: failed to read sport dma queue\n");
      return(-1);
   }
   
   *buffer = NULL;

   // return 0 if dma_buffer.done flag is not set
   // this means no data is available
   if(adc->dma_buffer.done == 0) {
      return(0);
   }

   // otherwise
   // set parameters for return
   //sec = (double)(tv.tv_sec) + (double)(tv.tv_usec)*0.000001;
   adc->time.tv_sec = adc->dma_buffer.tv.tv_sec;
   adc->time.tv_nsec = adc->dma_buffer.tv.tv_nsec;
   
   // if there's no time stamp on the dma buffer use current sys time
   if(adc->time.tv_sec == 0) {
	   gettimeofday(&now, NULL);
	   adc->time.tv_sec = now.tv_sec;
	   adc->time.tv_nsec = now.tv_usec*1000;
   }
   
   nsamps = adc->dma_buffer.size;
   
   // could also copy from dma buffer into a tmp buffer
   //size_t nbytes;
   //nbytes = 4 * (size_t)nsamps;
   //memcpy((u_int32_t *)(adc->dma_buffer.addr), (u_int32_t *)(adc->tmp_buffer), nbytes);
   //*buffer = (u_int32_t *)(adc->tmp_buffer);

   // set pointer to finished buffer
   *buffer = (u_int32_t *)(adc->dma_buffer.addr);
   
   // clear and update status
   adc->status = 0;
   if(adc->dma_buffer.overflow) {
	   adc->status |= ADC_BUFFER_OVERRUN;
       log_printf("sport_adc_read_dma: Buffer Overrun\n");
   }

   if(verbose_level > 1) {
	   double sec = (double)adc->time.tv_sec + 1e-9*(double)adc->time.tv_nsec;
     log_printf("sport_adc_read_dma: timestamp %0.3f\n", sec);
   }

   return(nsamps);
}

// 
// Read dma buffer into a data object, if it is ready.
// If a buffer is ready to read, return the number of bytes read.
// If dma buffer is not ready, return 0.
// The current dma buffer is set to data->buffer.
// The 32 bit data words are scaled according to the specified words size (data->nbps)
//
int sport_adc_read_dma_data(sport_adc_t *adc, sport_adc_data_t *data)
{
   struct timeval now;

   /* Configure sport dma queue */
   if (ioctl(adc->fd, SPORT_DMA_READ, &(adc->dma_buffer)) < 0) {
      log_printf("sport_adc_read_dma_data: failed to read sport dma queue\n");
      return(-1);
   }
   
   data->buffer = NULL;

   if(adc->dma_buffer.done == 0) {
      return(0);
   }

   // set parameters for return
   //sec = (double)(tv.tv_sec) + (double)(tv.tv_usec)*0.000001;
   data->time.tv_sec = adc->dma_buffer.tv.tv_sec;
   data->time.tv_usec = adc->dma_buffer.tv.tv_nsec/1000;
   
   // if there's no time stamp on the dma buffer use current time
   if(data->time.tv_sec == 0) {
	   gettimeofday(&now, NULL);
	   data->time.tv_sec = now.tv_sec;
	   data->time.tv_usec = now.tv_usec;
   }
   
   data->nsamps = adc->dma_buffer.size;
   data->fs = adc->fs;
   data->gain = adc->gain;
   data->nchans = adc->nchans;
   data->nbps = 32;  // until it is scaled

   // set data buffer pointer to finished dma buffer
   data->buffer = (void *)(adc->dma_buffer.addr);

   // clear and update status
   data->status = 0;
   if(adc->dma_buffer.overflow) {
	   data->status |= ADC_BUFFER_OVERRUN;
	   log_printf("sport_adc_read_dma_data: Buffer Overrun\n");
   }

   if(verbose_level > 1) {
     log_printf("sport_adc_read_dma_data: sec=%ld, usec=%ld\n", 
       data->time.tv_sec, data->time.tv_usec);
   }

   return(data->nsamps);
}


/*
 Reformat ad7766 data buffer into a 16 signed integer by shifting 
 the 24 bit word right (>>) by a specified number of bits (shift).
 The ad7766 buffers are 32 bit words with valid data in the last 24 bits.
 Bit 24 is a sign bit and the word is a two's compliment integer.
 A shift value less than 8, may cause overflow, so there's some extra checking.
*/
int16_t *sport_ad7766_reformat_int16(u_int32_t *buf, int nsamps, int shift)
{
   int n;
   int16_t *buf16 = (int16_t *)buf;
   if(shift < 0) shift = 0;
   // ad7766 returns a 24 bit 2-comp word in a 32 bit uint32
   // turn it into a int32 by shifting left and right to preserve the sign bit 
   if(shift < 8) {
      for (n = 0; n < nsamps; n++) {
         int32_t v = ((int32_t)(buf[n] << 8) >> (8 + shift));
         // now you cast it into an int16, but you need to check for overflow
         if(v > 32767) buf16[n] = 32767;
         else if (v < -32767) buf16[n] = -32767;
         else buf16[n] = (int16_t)v;
	  }
   } else {
	  // same as above, but no overflow check to speed it up
      for (n = 0; n < nsamps; n++) {
         int32_t v = ((int32_t)(buf[n] << 8) >> (8 + shift));
         buf16[n] = (int16_t)v;
	  }
   }
   return(buf16);
}

/*
 * Reformat 32 bit word into 24 bit word.
 */
unsigned char *sport_ad7766_reformat_int24(u_int32_t *buf, int nsamps)
{
   unsigned long n, i, j;
   unsigned char *buf32;
   unsigned char *buf24;
   buf32 = (unsigned char *)buf;
   buf24 = (unsigned char *)buf;
   for(n = 0; n < nsamps; n++) {
	   i = n*4;
	   j = n*3;
       buf24[j] = buf32[i]; 
       buf24[j+1] = buf32[i+1]; 
       buf24[j+2] = buf32[i+2]; 
   }
   return(buf24);
}

/*
 Reformat the 24 bit word read from the ad7766 into a 32 bit.
 The adc buffer has 32 bit words with valid data in the last 24 bits.
 Bit 24 is a sign bit (not bit 32) and the word is a two's compliment integer.
 The sign bit is preserved by shifting << 8 bits and then shifting >> 8.
*/
int32_t *sport_ad7766_reformat_int32(u_int32_t *buf, int nsamps)
{
   int n;
   int32_t *new = (int32_t *)buf;
   for(n = 0; n < nsamps; n++) {
   // cast the unsigned data word into a signed word before shifting >> 
   // otherwise, the sign bit is lost in the shift
      new[n] = (int32_t)(buf[n] << 8) >> 8; 
   }
   return(new);
}

//
// sport_adc_close
//
void sport_adc_close(sport_adc_t *adc)
{
   // turn ADC VREF OFF
   gpio_write(GPIO_VREF, 0);
   gpio_unexport(GPIO_VREF);

   // stop mclk signal
   ioctl (adc->mclk_fd, BFIN_TIMER_STOP);
   close(adc->mclk_fd);

   ioctl (adc->rsclk_fd, BFIN_TIMER_STOP);
   close(adc->rsclk_fd);

//   if(ADC_USE_NONKERNEL_MEMORY <= 0) {
//	   int n;
//	   for(n=0; n<(adc->nbufs); n++) {
//		   free(adc->buffer[n]);
//	   }
//   }

   close(adc->fd);
   //free(adc);
}

