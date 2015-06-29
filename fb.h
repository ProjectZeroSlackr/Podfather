/* framebuffer handling functions */

#ifndef __FB_H__
#define __FB_H__

int fb_open();
void fb_write(unsigned char *pixels);
int fb_quitting();
void fb_close();

#endif

