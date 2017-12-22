/*
 * beaked whale click detection
*/
#ifndef _DETECT_H
#define _DETECT_H

#include <fract.h>
#include <filter.h>
#include <complex.h>
#include <window.h>
#include "com.h"

// detection quality
#define DTX_BAD 0
#define DTX_GOOD 1
#define DTX_FAIR 2

#define DTX_LOG_FILE  "/mnt/detections.dtx"
#define DTX_COM_FILE  "/mnt/com.dtx"

// detection modes
#define DTX_NORMALIZE   0x01

// detection state flags
#define DETECT_PASSED			0x01
#define DETECT_PROCESSING		0x02
#define DETECT_ERROR			0x04
#define DETECT_FAILED			0x08
#define DETECT_FAILED_ICI		0x10
#define DETECT_FAILED_MIN_GOOD	0x20
#define DETECT_FAILED_LOG		0x40
#define DETECT_FAILED_COM_LOG	0x80

// click structure
struct click_struct {
   float thresh;
   float ratio;
   float tke;
   float ici;
   double time;
   char quality;
};
typedef struct click_struct click_t;

struct click_detect_struct {
   int mode;      // mode
   int nclicks;  // min number of clicks
   int nwins;    // number of windows
   int winsize;  // filter window size [num samples]
   float ici[2];  // ici (min,max) [sec]
   float thresh_factor; // ration thresh
   float ici_thresh; // ici fractional thresh, [number of good/total clicks]
   //int bitshift; // bitshift to apply to data word before detection
};
typedef struct click_detect_struct click_detect_t;

extern int init_detect(int min_num_clicks, int nsamps_per_buf, int nbufs_per_detection, 
	float ici_thresh, float thresh_factor, float ici_min, float ici_max);

extern int detect(int16_t *buf, int nsamps, int fs, double t0, int *status);

extern int com_send_dtx(com_t *com, int max_ndtx);

#endif /* _DETECT_H */

