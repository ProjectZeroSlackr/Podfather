#include <unistd.h>
#include <mikmod.h>
#include <stdlib.h>
#include <sys/time.h>

#ifndef __IPOD__
#include <SDL/SDL.h>
#endif

#include "podfather.h"
#include "fb.h"
#include "rotazoom.h"
#include "shadeplasma.h"
#include "clovermap.h"
#include "copper.h"
#include "vectorballs.h"
#include "bobs.h"
#include "tunnel.h"
#include "rubber.h"
#include "end.h"

int main(int argc, char *argv[])
{
	MODULE *module;
	struct timeval start_time, curr_time;

	int has_music, is_running;
	long time;
	
	/* precalc */
	printf("*** Loading... ***\n");
	rotazoom_init();
	shadeplasma_init();
	clovermap_init();
	copper_init();
	vectorballs_init();
	bobs_init();
	tunnel_init();
	rubber_init();
	end_init();

	fb_open();

	/* register all the drivers */
	MikMod_RegisterAllDrivers();

	/* register all the module loaders */
	MikMod_RegisterAllLoaders();

	/* initialize mikmod */
	md_mode |= DMODE_SOFT_MUSIC;
	md_mixfreq = 22100;
	has_music = !MikMod_Init("");
	if (has_music) {
		/* load module */
		module = Player_Load("fif.mod", 64, 0);
		if (!module) {
			fprintf(stderr, "Warning: Could not load module (%s)\n",
			MikMod_strerror(MikMod_errno));
		}
	} else {
		fprintf(stderr, "Warning: Could not initialize sound (%s)\n",
			MikMod_strerror(MikMod_errno));
	}

	if (has_music && module) {
		/* start module */
		Player_Start(module);
	}
	
	gettimeofday(&start_time, NULL);

	is_running = 1;
	
	while (is_running) {
		usleep(1000);
		gettimeofday(&curr_time, NULL);
		
		time = (curr_time.tv_sec - start_time.tv_sec) * 1000 + (curr_time.tv_usec - start_time.tv_usec) / 1000;
		
		if (time < PATTERN * 4) {
			shadeplasma_frame(time);
		} else if (time < PATTERN * 8) {
			clovermap_frame(time - PATTERN * 4);
		} else if (time < PATTERN * 12) {
			copper_frame(time - PATTERN * 8);
		} else if (time < PATTERN * 16) {
			rotazoom_frame(time - PATTERN * 12);
		} else if (time < PATTERN * 24) {
			bobs_frame(time - PATTERN * 16);
		} else if (time < PATTERN * 28) {
			rubber_frame(time - PATTERN * 24);
		} else if (time < PATTERN * 36) {
			vectorballs_frame(time - PATTERN * 28);
		} else if (time < PATTERN * 40) {
			tunnel_frame(time - PATTERN * 36);
		} else {
			end_frame(time - PATTERN * 40);
		}
		
		if (fb_quitting()) is_running = 0;
		
		if (has_music && module) {
			MikMod_Update();
			is_running &= Player_Active();
		}
	}
	
	fb_close();
	
	if (has_music && module) {
		Player_Stop();
		Player_Free(module);
	}

	MikMod_Exit();

	return 0;
}
