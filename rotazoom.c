#include <math.h>

#include "podfather.h"
#include "rotazoom.h"
#include "fb.h"

static unsigned char pixels[LCD_HEIGHT * LCD_WIDTH];

static int sine[256];

static unsigned char bmp[] = {
	0,0,0,0,0,3,3,3,3,3,0,0,0,0,0,0,
	0,0,0,3,3,2,2,2,2,2,3,3,0,0,0,0,
	0,0,3,2,2,1,1,2,1,2,2,3,3,0,0,0,
	0,3,2,1,1,1,2,1,2,2,2,2,3,3,0,0,
	0,3,2,1,1,1,1,1,1,2,1,2,2,3,0,0,
	3,2,1,1,1,1,2,1,2,2,2,2,3,3,3,0,
	3,2,1,1,1,1,1,1,1,2,1,2,2,3,3,0,
	3,2,2,1,2,1,2,1,2,2,2,2,3,3,3,0,
	3,2,1,2,1,1,1,2,1,2,2,2,2,3,3,0,
	3,2,2,2,2,2,2,2,2,2,2,3,3,3,3,0,
	0,3,2,2,1,2,1,2,2,2,2,3,2,3,0,0,
	0,3,3,2,2,2,2,2,2,3,3,3,3,3,0,0,
	0,0,3,3,2,3,2,3,2,3,2,3,3,0,0,0,
	0,0,0,3,3,3,3,3,3,3,3,3,0,0,0,0,
	0,0,0,0,0,3,3,3,3,3,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

void rotazoom_init() {
	int i;
	
	for (i=0; i<256; i++) {
		sine[i] = (int)(0x7fff * sin(i * M_PI / 128));
	}
}

void rotazoom_frame(long time) {
	unsigned char *pixpos;
	int theta = (time / 34) & 0xff;
	/* int scale = 0xc0; */
	int scale = ((0x9000 + sine[(time / 45) & 0xff]) >> 8);
	int xdisp, ydisp, ibmpx, ibmpy, bmpx, bmpy, x, y;

	xdisp = (sine[(theta + 0x40) & 0xff] * scale) >> 16;
	ydisp = (sine[theta] * scale) >> 16;
	
	ibmpx = sine[(time / 30) & 0xff] / 2;
	ibmpy = 0;
	
	pixpos = pixels;
	for (y = 0; y < LCD_HEIGHT; y++) {
		bmpx = ibmpx; bmpy = ibmpy;
		for (x = 0; x < LCD_WIDTH; x++) {
			*pixpos = bmp[((bmpx >> 8) & 0x0f) | ((bmpy >> 4) & 0xf0)];
			bmpx += xdisp; bmpy += ydisp;
			pixpos++;
		}
		ibmpy += xdisp; ibmpx -= ydisp;
	}
	fb_write(pixels);
}
