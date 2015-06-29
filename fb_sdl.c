/* framebuffer implementation for SDL */

#include "fb.h"
#include "podfather.h"
#include <SDL/SDL.h>
#include <string.h>

SDL_Color white = {255, 255, 255, 0};
SDL_Color ltgrey = {192, 192, 192, 0};
SDL_Color dkgrey = {128, 128, 128, 0};
SDL_Color black = {0, 0, 0, 0};

SDL_Color ipod_palette[256];

static SDL_Surface *screen;

int fb_open() {
	int i;

	SDL_Init(SDL_INIT_VIDEO);

	screen = SDL_SetVideoMode(LCD_WIDTH, LCD_HEIGHT, 8, SDL_SWSURFACE);

	if (!screen) {
		SDL_Quit();
		printf ("Could not set 160x128x8 video mode: %s\n", SDL_GetError());
		return 1;
	}

	/* set palette */
	for (i = 0; i < 256; i+= 4) {
		ipod_palette[i] = white;
		ipod_palette[i + 1] = ltgrey;
		ipod_palette[i + 2] = dkgrey;
		ipod_palette[i + 3] = black;
	}
	SDL_SetColors(screen, ipod_palette, 0, 256);
	
	SDL_ShowCursor(SDL_DISABLE);

	return 0;
}

void fb_write(unsigned char *pixels) {
	void *sdl_pixels = screen->pixels;
	SDL_LockSurface(screen);
	memcpy(sdl_pixels, (void *)pixels, LCD_WIDTH * LCD_HEIGHT);
	SDL_UnlockSurface(screen);
	SDL_Flip(screen);
}

int fb_quitting() {
	SDL_Event event;

	while( SDL_PollEvent( &event ) ){
		switch( event.type ){
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_m || event.key.keysym.sym == SDLK_COMMA) {
					return 1;
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

void fb_close() {
	SDL_Quit();
}
