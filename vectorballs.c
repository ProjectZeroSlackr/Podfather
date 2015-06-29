#include "vectorballs.h"

#include "sprite.h"
#include "podfather.h"
#include "fb.h"
#include "ducks.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static unsigned char pixels[LCD_HEIGHT * LCD_WIDTH];

static unsigned char duck_anim[64] = {
	1,1,0,0,1,1,0,0,1,1,0,0,1,0,1,1,
	1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
	0,0,0,0,1,1,1,1,0,0,1,1,1,1,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0
};

static unsigned char ball_pixels[] = {
	0,0,2,2,3,3,0,0,
	0,2,1,1,2,2,3,0,
	2,1,0,1,1,2,2,3,
	2,1,1,1,1,2,3,3,
	3,2,1,1,2,2,3,3,
	3,2,2,2,2,3,3,3,
	0,3,2,3,3,3,3,0,
	0,0,3,3,3,3,0,0
};

static unsigned char ball_mask[] = {
	0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,
	0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0xff,0x00,0x00,0x00,0x00,0x00,0x00,0xff,
	0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff
};

static bitmap ball = {
	ball_pixels,
	ball_mask,
	8,
	8
};

#define VERTEX_COUNT 56

/* static int cube[VERTEX_COUNT][3] = {
	{-0x100, -0x100, -0x100},
	{-0x100, -0x100, 0x100},
	{-0x100, 0x100, -0x100},
	{-0x100, 0x100, 0x100},
	{0x100, -0x100, -0x100},
	{0x100, -0x100, 0x100},
	{0x100, 0x100, -0x100},
	{0x100, 0x100, 0x100}
}; */

static int object[2][VERTEX_COUNT][3] = {
	{ /* teapot */
		{3,278,12},
		{40,235,12},
		{-23,235,38},
		{3,235,-25},
		{226,-195,9},
		{200,-181,125},
		{-111,-194,-184},
		{-163,-186,-157},
		{272,-18,52},
		{272,-150,52},
		{363,11,52},
		{385,37,22},
		{420,60,9},
		{495,137,36},
		{-237,96,34},
		{-367,91,34},
		{-413,60,34},
		{-410,-34,2},
		{-289,-119,2},
		{240,179,10},
		{250,137,80},
		{132,137,235},
		{0,179,250},
		{-69,137,261},
		{-224,137,142},
		{-240,179,10},
		{-250,138,-59},
		{-132,138,-214},
		{0,180,-229},
		{69,138,-240},
		{170,180,-159},
		{224,138,-121},
		{278,96,10},
		{197,96,208},
		{0,96,288},
		{-197,96,208},
		{-278,96,10},
		{-197,96,-187},
		{0,96,-268},
		{197,96,-187},
		{316,-23,10},
		{224,-23,235},
		{0,-23,326},
		{-224,-23,235},
		{-316,-23,10},
		{-224,-22,-214},
		{0,-22,-306},
		{224,-22,-214},
		{299,-120,10},
		{212,-120,222},
		{0,-120,309},
		{-212,-120,222},
		{-299,-120,10},
		{-212,-120,-202},
		{0,-120,-289},
		{212,-120,-202}
	},
	{ /* duck */
		{66,105,486},
		{-66,105,486},
		{-51,105,564},
		{51,105,564},
		{5,-301,-153},
		{5,-232,-488},
		{-114,-232,-390},
		{-164,-232,-153},
		{-114,-232,83},
		{5,-232,181},
		{125,-232,83},
		{174,-232,-153},
		{125,-232,-390},
		{5,-65,-627},
		{-164,-65,-488},
		{-234,-65,-153},
		{-164,-65,181},
		{5,-65,320},
		{174,-65,181},
		{244,-65,-153},
		{174,-65,-488},
		{5,101,-488},
		{-114,101,-390},
		{-164,101,-153},
		{-114,101,83},
		{5,101,181},
		{125,101,83},
		{174,101,-153},
		{125,101,-390},
		{5,170,-153},
		{0,0,294},
		{135,59,294},
		{96,59,211},
		{0,59,176},
		{-96,59,211},
		{-135,59,294},
		{-96,59,377},
		{0,59,412},
		{96,59,377},
		{192,204,294},
		{135,204,176},
		{0,204,128},
		{-135,204,176},
		{-192,204,294},
		{-135,204,412},
		{0,204,460},
		{135,204,412},
		{135,349,294},
		{96,349,211},
		{0,349,176},
		{-96,349,211},
		{-135,349,294},
		{-96,349,377},
		{0,349,412},
		{96,349,377},
		{0,409,294}
	}
};

static int sine[256];

static int rota[9] = {
	0x100, 0, 0,
	0, 0, 0,
	0, 0, 0
};
static int rotb[9] = {
	0, 0, 0,
	0, 0x100, 0,
	0, 0, 0
};
static int rotc[9] = {
	0, 0, 0,
	0, 0, 0,
	0, 0, 0x100
};

static int rotab[9];
static int rotabc[9];

static void mat3mul(int *m1, int *m2, int *r) {
	r[0] = (m1[0] * m2[0] + m1[1] * m2[3] + m1[2] * m2[6]) >> 8;
	r[1] = (m1[0] * m2[1] + m1[1] * m2[4] + m1[2] * m2[7]) >> 8;
	r[2] = (m1[0] * m2[2] + m1[1] * m2[5] + m1[2] * m2[8]) >> 8;
	r[3] = (m1[3] * m2[0] + m1[4] * m2[3] + m1[5] * m2[6]) >> 8;
	r[4] = (m1[3] * m2[1] + m1[4] * m2[4] + m1[5] * m2[7]) >> 8;
	r[5] = (m1[3] * m2[2] + m1[4] * m2[5] + m1[5] * m2[8]) >> 8;
	r[6] = (m1[6] * m2[0] + m1[7] * m2[3] + m1[8] * m2[6]) >> 8;
	r[7] = (m1[6] * m2[1] + m1[7] * m2[4] + m1[8] * m2[7]) >> 8;
	r[8] = (m1[6] * m2[2] + m1[7] * m2[5] + m1[8] * m2[8]) >> 8;
}

void vectorballs_init() {
	int i;
	
	for (i=0; i<256; i++) {
		sine[i] = (int)(0xff * sin(i * M_PI / 128));
	}
}

void vectorballs_frame(long time) {
	int ar = time * 70 / 1000;
	int br = time * 65 / 1000;
	int cr = time * 59 / 1000;
	
	int i, x, y, objno;
	
	/* clear screen */
	memset(pixels, 0, LCD_WIDTH * LCD_HEIGHT);
	
	rota[4] = rota[8] = sine[(ar + 0x40) & 0xff];
	rota[5] = -sine[ar & 0xff];
	rota[7] = sine[ar & 0xff];
	
	rotb[0] = rotb[8] = sine[(br + 0x40) & 0xff];
	rotb[2] = -sine[br & 0xff];
	rotb[6] = sine[br & 0xff];
	
	rotc[0] = rotc[4] = sine[(cr + 0x40) & 0xff];
	rotc[1] = -sine[cr & 0xff];
	rotc[3] = sine[cr & 0xff];
	
	mat3mul(rota, rotb, rotab);
	mat3mul(rotab, rotc, rotabc);
	
	objno = (time < PATTERN * 4) ? 0 : 1;
	
	for (i = 0; i < VERTEX_COUNT; i++) {
		x = (object[objno][i][0] * rotabc[0] + object[objno][i][1] * rotabc[1] + object[objno][i][2] * rotabc[2]) * (LCD_HEIGHT / 4) >> 16;
		y = (object[objno][i][0] * rotabc[3] + object[objno][i][1] * rotabc[4] + object[objno][i][2] * rotabc[5]) * (LCD_HEIGHT / 4) >> 16;
		centre_sprite(pixels, &ball, x + LCD_WIDTH / 2, y + LCD_HEIGHT / 2);
	}
	
	if (objno == 1) {
		/* objno 1 = duck, so show duck sprites */
		if (duck_anim[((time * 64) / PATTERN) % 64]) {
			put_sprite(pixels, &duck_left_open, 8, LCD_HEIGHT - 8 - duck_left_open.height);
			put_sprite(pixels, &duck_right_open, LCD_WIDTH - 8 - duck_right_open.width, LCD_HEIGHT - 8 - duck_right_open.height);
		} else {
			put_sprite(pixels, &duck_left_closed, 8, LCD_HEIGHT - 8 - duck_left_closed.height);
			put_sprite(pixels, &duck_right_closed, LCD_WIDTH - 8 - duck_right_closed.width, LCD_HEIGHT - 8 - duck_right_closed.height);
		}
	}
	
	fb_write(pixels);
}
