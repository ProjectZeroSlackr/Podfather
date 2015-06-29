#include "end.h"

#include "fb.h"
#include "sprite.h"
#include "text_printer.h"
#include "podfather.h"

#include <stdio.h>
#include <strings.h>

static unsigned char pixels[LCD_HEIGHT * LCD_WIDTH];

static bitmap *text1_bmp;
static unsigned char *text1[] = {"code", "gasman", "", "music", "yerzmyey", "", "created for", "sundown", NULL};

void end_init() {
	text1_bmp = render_text(text1);
}

void end_frame(long time) {
	/* clear screen */
	memset(pixels, 0, LCD_WIDTH * LCD_HEIGHT);

	if (time < PATTERN * 2) {
		centre_sprite(pixels, text1_bmp, LCD_WIDTH / 2, LCD_HEIGHT / 2);
	}
	
	fb_write(pixels);
}
