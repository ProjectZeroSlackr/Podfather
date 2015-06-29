#include "copper.h"

#include "fb.h"
#include "podfather.h"
#include "text_printer.h"
#include "sprite.h"
#include <string.h>
#include <math.h>

static unsigned char bar[] = {3,2,1,2,0,1,0,1,2,1,3};
static unsigned char pixels[LCD_HEIGHT * LCD_WIDTH];
static unsigned char line[LCD_WIDTH];
static int sine[256];

static unsigned char* scrolltext[] = {"booyakasha! hooy program are back on the scene with the world\'s first ipod demo!", NULL};
static bitmap *scrolltext_bmp;

void copper_init() {
	int i;
	
	for (i=0; i<256; i++) {
		sine[i] = (int)(0x7fff * sin(i * M_PI / 128));
	}
	
	scrolltext_bmp = render_text(scrolltext);
}

void copper_frame(long time) {
	int y = 0;
	int yaddr = 0;
	int x, s1, s2;
	
	s1 = ((time * 5) >> 5) & 0xff; s2 = ((time * 6) >> 5) & 0xff;
	
	/* clear line for new frame */
	memset(line, 0, LCD_WIDTH);
	
	for (y = 0; y < LCD_HEIGHT; y++) {
		if (y < LCD_HEIGHT * 2 / 3) {
			/* determine x pos */
			x = (((sine[s1 & 0xff] + sine[s2 & 0xff]) * (LCD_WIDTH - 11) / 2) >> 16) + (LCD_WIDTH - 11) / 2;
			/* copy bar graphic to line */
			memcpy(line + x, bar, 11);
			s1 += 2; s2 += 3;
		}

		/* copy line graphic to screen buffer */
		memcpy(pixels + yaddr, line, LCD_WIDTH);
		yaddr += LCD_WIDTH;
	}
	
	put_sprite(pixels, scrolltext_bmp,
		(LCD_WIDTH - (time / 11)) + (sine[(time / 7) & 0xff] >> 10),
		80 + (sine[(time / 5) & 0xff] / 3600));
	
	fb_write(pixels);
}
