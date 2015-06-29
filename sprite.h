/* sprite display routines */

#ifndef __SPRITE_H__
#define __SPRITE_H__

typedef struct _bitmap {
	unsigned char *pixels;
	unsigned char *mask;
	int width;
	int height;
} bitmap;

void put_sprite(unsigned char *screen, bitmap *sprite, int x0, int y0);
void centre_sprite(unsigned char *screen, bitmap *sprite, int x0, int y0);

#endif
