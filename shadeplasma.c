#include <math.h>

#include "podfather.h"
#include "shadeplasma.h"
#include "fb.h"
#include "text_printer.h"
#include "sprite.h"
#include <stdio.h>

static unsigned char pixels[LCD_HEIGHT * LCD_WIDTH];

static int sine[256];

static int val2shade[] = {
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2,
	 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3,15, 3,15, 3,
	15, 3,15, 3,15, 3,15, 3,15, 3,15, 3,15, 3,15, 4,
	13, 4,13, 4,13, 4,13, 4,13, 4,13, 4,13, 4,13, 4,
	13, 4,13, 5,16, 5,16, 5,16, 5,16, 5,16, 5,16, 5,
	16, 5,16, 5,16, 5,16, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7,17, 7,17, 7,
	17, 7,17, 7,17, 7,17, 7,17, 7,17, 7,17, 7, 8,14,
	 8,14, 8,14, 8,14, 8,14, 8,14, 8,14, 8,14, 8,14,
	 8,14, 9,18, 9,18, 9,18, 9,18, 9,18, 9,18, 9,18,
	 9,18, 9,18, 9,18,10,10,10,10,10,10,10,10,10,10,
	10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,
	11,11,11,11,11,11,11,11,11,11,11,11,11,12,12,12,
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12
};

static unsigned char shades[19][4] = {
	{0,0,0,0}, {0,0,0,1}, {1,0,0,1}, {1,0,1,1},
	{1,1,1,1}, {1,1,1,2}, {2,1,1,2}, {2,1,2,2},
	{2,2,2,2}, {2,2,2,3}, {3,2,2,3}, {3,2,3,3},
	{3,3,3,3},
	{1,0,2,1}, {2,1,3,2}, {1,0,0,2}, {2,0,1,2},
	{2,1,1,3}, {3,1,2,3}
};

static bitmap *text1_bmp;
static unsigned char *text1[] = {"fear", "the pod", NULL};
static bitmap *text2_bmp;
static unsigned char *text2[] = {"for it", "is shiny", NULL};

void shadeplasma_init() {
	int i;
	
	for (i=0; i<256; i++) {
		sine[i] = (int)(0x7fff * sin(i * M_PI / 128));
	}
	
	text1_bmp = render_text(text1);
	text2_bmp = render_text(text2);
}

void shadeplasma_frame(long time) {
	unsigned char *pixpos;
	int x,y;
	int x1i = ((time * -10) >> 7) & 0xff;
	int x2i = ((time * 17) >> 7) & 0xff;
	int y1i = ((time * 13) >> 7) & 0xff;
	int y2i = ((time * -23) >> 7) & 0xff;
	int y3i = ((time * 8) >> 7) & 0xff;

	int x1, x2, y1, y2, y3;
	int i;
	int text_scale;
	
	pixpos = pixels;
	y1 = y1i; y2 = y2i; y3 = y3i;
	for (y = 0; y < LCD_HEIGHT; y += 2) {
		x1 = (x1i + (sine[y3] >> 9)) & 0xff ; x2 = (x2i + (sine[y3] >> 9)) & 0xff;
		for (x = 0; x < LCD_WIDTH; x += 2) {
			i = val2shade[((sine[x1] + sine[x2] + sine[y1] + sine[y2]) >> 10) + 0x80];
			pixpos[0] = shades[i][0];
			pixpos[1] = shades[i][1];
			pixpos[LCD_WIDTH] = shades[i][2];
			pixpos[LCD_WIDTH + 1] = shades[i][3];
			x1 = (x1 + 7) & 0xff; x2 = (x2 + 3) & 0xff;
			pixpos += 2;
		}
		pixpos += LCD_WIDTH;
		y1 = (y1 + 5) & 0xff; y2 = (y2 + 2) & 0xff; y3 = (y3 + 2) & 0xff;
	}
	
	if (time < 1500) {
		text_scale = (1500 - time) * LCD_WIDTH / 1000;
	} else if (time < 6000) {
		text_scale = 0;
	} else if (time < 7500) {
		text_scale = (time - 6000) * LCD_WIDTH / 1000;
	} else if (time < 9000) {
		text_scale = (9000 - time) * LCD_WIDTH / 1000;
	} else if (time < 13500) {
		text_scale = 0;
	} else {
		text_scale = (time - 13500) * LCD_WIDTH / 1000;
	}
	
	centre_sprite(pixels, (time < 7500 ? text1_bmp : text2_bmp),
		LCD_WIDTH / 2 + ((text_scale * sine[(time >> 2) & 0xff]) >> 16),
		LCD_HEIGHT / 2 + ((text_scale * sine[((time >> 2) + 0x40) & 0xff]) >> 16));
	
	
	fb_write(pixels);
}
