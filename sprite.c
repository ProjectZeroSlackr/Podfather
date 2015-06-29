#include "sprite.h"
#include "podfather.h"

void put_sprite(unsigned char *screen, bitmap *sprite, int x0, int y0) {
	int x, y, spr_offset, scr_offset;
	
	for (y = 0; y < sprite->height; y++) {
		if (y + y0 >= 0 && y + y0 < LCD_HEIGHT) {
			for (x = 0; x < sprite->width; x++) {
				if (x + x0 >= 0 && x + x0 < LCD_WIDTH) {
					scr_offset = (y + y0) * LCD_WIDTH + x + x0;
					spr_offset = y * sprite->width + x;
					screen[scr_offset] = (screen[scr_offset] & sprite->mask[spr_offset]) | sprite->pixels[spr_offset];
				}
			}
		}
	}
}

void centre_sprite(unsigned char *screen, bitmap *sprite, int x0, int y0) {
	put_sprite(screen, sprite, x0 - sprite->width / 2, y0 - sprite->height / 2);
}
