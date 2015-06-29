#include "clovermap.h"

#include "podfather.h"
#include "fb.h"
#include "sprite.h"
#include <math.h>

#include "podfather_title.h"

static int scrmap_x[LCD_HEIGHT][LCD_WIDTH];
static int scrmap_y[LCD_HEIGHT][LCD_WIDTH];
static unsigned char pixels[LCD_HEIGHT * LCD_WIDTH];

static int sine[256];

static unsigned char texture[16][16] = {
	{1,1,1,1,3,2,2,2,2,2,2,2,0,1,1,1},
	{1,1,1,3,3,3,2,2,2,2,2,0,0,0,1,1},
	{1,1,3,3,3,3,3,2,2,2,0,0,0,0,0,1},
	{1,3,3,3,3,3,3,3,2,0,0,0,0,0,0,0},
	{2,3,3,3,3,3,3,3,1,0,0,0,0,0,0,0},
	{2,2,3,3,3,3,3,1,1,1,0,0,0,0,0,2},
	{2,2,2,3,3,3,1,1,1,1,1,0,0,0,2,2},
	{2,2,2,2,3,1,1,1,1,1,1,1,0,2,2,2},
	{2,2,2,2,0,1,1,1,1,1,1,1,3,2,2,2},
	{2,2,2,0,0,0,1,1,1,1,1,3,3,3,2,2},
	{2,2,0,0,0,0,0,1,1,1,3,3,3,3,3,2},
	{2,0,0,0,0,0,0,0,1,3,3,3,3,3,3,3},
	{1,0,0,0,0,0,0,0,2,3,3,3,3,3,3,3},
	{1,1,0,0,0,0,0,2,2,2,3,3,3,3,3,1},
	{1,1,1,0,0,0,2,2,2,2,2,3,3,3,1,1},
	{1,1,1,1,0,2,2,2,2,2,2,2,3,1,1,1}
};

/* static unsigned char texture[16][16] = {
	{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
	{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
	{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
	{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
	{1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1},
	{1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1},
	{1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1},
	{1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1},
	{1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1},
	{1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1},
	{1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1},
	{1,1,1,1,3,3,3,3,3,3,3,3,1,1,1,1},
	{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
	{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
	{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0},
	{0,0,0,0,2,2,2,2,2,2,2,2,0,0,0,0}
}; */

#define TEX_WIDTH 128
#define TEX_HEIGHT 128
#define TEX_SCALE 8

void clovermap_init() {
	int x, y;
	
	int cx = LCD_WIDTH / 2;
	int cy = LCD_HEIGHT / 2;
	
	double th, r, s;
	
	int i;
	
	for (i=0; i<256; i++) {
		sine[i] = (int)(0x7fff * sin(i * M_PI / 128));
	}

	for (y = 0; y < LCD_HEIGHT; y++) {
		for (x = 0; x < LCD_WIDTH; x++) {
			th = atan2(x - cx, cy - y);
			r = sqrt((x-cx)*(x-cx) + (y-cy)*(y-cy));
			s = (1.5 + cos(5 * th)) * (2.5 * LCD_HEIGHT / 3);
			scrmap_x[y][x] = (int)floor(th * TEX_WIDTH / (2 * M_PI) + TEX_WIDTH) % TEX_WIDTH;
			scrmap_y[y][x] = (int)floor(TEX_HEIGHT * r / s) % TEX_HEIGHT;
		}
	}
}

void clovermap_frame(long time) {
	int x, y;
	
	int yoff = sine[(time / 4) & 0xff] * (TEX_HEIGHT / 4) >> 15;
	int xoff = time * TEX_WIDTH / 4000;

	for (y = 0; y < LCD_HEIGHT; y++) {
		for (x = 0; x < LCD_WIDTH; x++) {
			pixels[y * LCD_WIDTH + x] = texture[((scrmap_y[y][x] + yoff) % TEX_HEIGHT) / TEX_SCALE][((scrmap_x[y][x] + xoff) % TEX_WIDTH) / TEX_SCALE];
		}
	}
	
	centre_sprite(pixels, &podfather_title, LCD_WIDTH / 2, LCD_HEIGHT / 2);
	
	fb_write(pixels);
}