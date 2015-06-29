#include "rubber.h"

#include "ipod.h"
#include "podfather.h"
#include "fb.h"
#include <math.h>
#include <string.h>

static int sine[256];
static unsigned char pixels[LCD_HEIGHT * LCD_WIDTH];

void rubber_init() {
	int i;
	
	for (i=0; i<256; i++) {
		sine[i] = (int)(0xffff * sin(i * M_PI / 128));
	}
}

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

void rubber_frame(long time) {
	int y, x, i;
	int sinrot, cosrot, rota;
	int lwid, rwid, lwidr, rwidr, texpos, shade;
	
	/* clear screen */
	memset(pixels, 0, LCD_WIDTH * LCD_HEIGHT);

	for (y = 0; y < 64; y++) {
		rota = (sine[(y + time / 10) & 0xff] + sine[(y + time / 12) & 0xff]) >> 10;
		/* rota = y + time / 40; */
		sinrot = sine[rota & 0x3f];
		cosrot = sine[(rota & 0x3f) + 0x40];
		lwid = (sinrot * 32) >> 16;
		rwid = (cosrot * 32) >> 16;
		x = (LCD_WIDTH / 2 - lwid - rwid) / 2;
		texpos = 0;
		lwidr = 0x200000 / lwid;
		for (i = 0; i < lwid; i++) {
			texpos += lwidr;
			shade = val2shade[((ipod[y][(texpos >> 16) & 31] * sinrot) >> 16) ^ 0xff];
			pixels[(y * 2) * LCD_WIDTH + x * 2] = shades[shade][0];
			pixels[(y * 2) * LCD_WIDTH + x * 2 + 1] = shades[shade][1];
			pixels[(y * 2 + 1) * LCD_WIDTH + x * 2] = shades[shade][2];
			pixels[(y * 2 + 1) * LCD_WIDTH + x * 2 + 1] = shades[shade][3];
			x++;
		}
		texpos = 0;
		rwidr = 0x200000 / rwid;
		for (i = 0; i < rwid; i++) {
			texpos += rwidr;
			shade = val2shade[((ipod[y][(texpos >> 16) & 31] * cosrot) >> 16) ^ 0xff];
			pixels[(y * 2) * LCD_WIDTH + x * 2] = shades[shade][0];
			pixels[(y * 2) * LCD_WIDTH + x * 2 + 1] = shades[shade][1];
			pixels[(y * 2 + 1) * LCD_WIDTH + x * 2] = shades[shade][2];
			pixels[(y * 2 + 1) * LCD_WIDTH + x * 2 + 1] = shades[shade][3];
			x++;
		}
	}
	
	fb_write(pixels);
}
