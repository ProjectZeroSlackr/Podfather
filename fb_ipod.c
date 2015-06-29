/* framebuffer implementation for iPod */
/* Hacked up from code copyright (C) 2004 Bernard Leach */

#include "fb.h"
#include "podfather.h"

#include <ctype.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define FBIOGET_BACKLIGHT	_IOR('F', 0x24, int)
#define FBIOSET_BACKLIGHT	_IOW('F', 0x25, int)

#define FB_DEV_NAME		"/dev/fb0"
#define FB_DEVFS_NAME		"/dev/fb/0"

#define inl(p) (*(volatile unsigned long *) (p))
#define outl(v,p) (*(volatile unsigned long *) (p) = (v))

#define BUFSIZR 512
static unsigned long ipod_lcd_base	= 0xc0001000;
static unsigned long ipod_rtc		= 0xcf001110;

static long get_generation(void) {
	int i;
	char cpuinfo[BUFSIZR];
	char *ptr;
	FILE *file;

	if ((file = fopen("/proc/cpuinfo", "r")) != NULL) {
		while (fgets(cpuinfo, sizeof(cpuinfo), file) != NULL)
			if (strncmp(cpuinfo, "Revision", 8) == 0)
				break;
		fclose(file);
	}
	for (i = 0; !isspace(cpuinfo[i]); i++);
	for (; isspace(cpuinfo[i]); i++);
	ptr = cpuinfo + i + 2;

	return strtol(ptr, NULL, 10);
}

static int ipod_ioctl(int request, int *arg) {
	int fd;

	fd = open(FB_DEV_NAME, O_NONBLOCK);
	if (fd < 0) fd = open(FB_DEVFS_NAME, O_NONBLOCK);
	if (fd < 0) {
		return -1;
	}
	if (ioctl(fd, request, arg) < 0) {
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

/* get current usec counter */
static int timer_get_current(void) {
	return inl(ipod_rtc);
}

/* check if number of useconds has passed */
static int timer_check(int clock_start, int usecs) {
	if ( (inl(ipod_rtc) - clock_start) >= usecs ) {
		return 1;
	} else {
		return 0;
	}
}

/* wait for LCD with timeout */
static void lcd_wait(void) {
	/* exit immediately (no need to wait) if lowest bit of ipod_lcd_base is 0 */
	if ( (inl(ipod_lcd_base) & 0x1) != 0 ) {
		int start = timer_get_current();

		/* wait for up to one millisecond for bit 0x8000 of ipod_lcd_base to be reset */
		do {
			if ( (inl(ipod_lcd_base) & (unsigned int)0x8000) == 0 ) break;
		} while ( timer_check(start, 1000) == 0 );
	}
}

int fb_open() {
	/* if 4th gen or above, use new memory base addresses */
	if (get_generation() > 40000) {
		ipod_lcd_base	= 0x70003000;
		ipod_rtc	= 0x60005010;
	}
	return ipod_ioctl(FBIOSET_BACKLIGHT, (int *) 1);
}

void fb_write(unsigned char *pixels) {
	int y, x;
	unsigned char *pixpos = pixels;
	
	for (y = 0; y < LCD_HEIGHT; y++) {
		/* send 'move' command */
		lcd_wait();
		outl(0x00, ipod_lcd_base + 0x8);
		lcd_wait();
		outl(0x11, ipod_lcd_base + 0x8); /* 0x11 = move */

		lcd_wait();
		outl(y >> 3, ipod_lcd_base + 0x10);
		lcd_wait();
		outl((y << 5) & 0xff, ipod_lcd_base + 0x10);

		/* send 'print' command */
		lcd_wait();
		outl(0x0, ipod_lcd_base + 0x8);
		lcd_wait();
		outl(0x12, ipod_lcd_base + 0x8); /* 0x12 = print */

		for (x = 0; x < (LCD_WIDTH >> 3); x++) {
			lcd_wait();
			outl((pixpos[7] & 0x03) << 6 | (pixpos[6] & 0x03) << 4 | (pixpos[5] & 0x03) << 2 | (pixpos[4] & 0x03), ipod_lcd_base + 0x10);
			lcd_wait();
			outl((pixpos[3] & 0x03) << 6 | (pixpos[2] & 0x03) << 4 | (pixpos[1] & 0x03) << 2 | (pixpos[0] & 0x03), ipod_lcd_base + 0x10);
			pixpos += 8;
		}
	}
}

int fb_quitting() {
	return 0;
}

void fb_close() {
	ipod_ioctl(FBIOSET_BACKLIGHT, (int *) 0);
}
