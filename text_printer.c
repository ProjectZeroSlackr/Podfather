/* text printer */

#include "text_printer.h"

#include "aluminium_font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int line_width(unsigned char *line) {
	int width = 0;
	while (*line != '\0') {
		width += strlen(aluminium_font[*line][0]);
		line++;
	}
	return width;
}

bitmap *render_text(unsigned char** text) {
	/* determine dimensions of bitmap */
	bitmap *b;
	int max_width = 0;
	int height = 0;
	int width;
	unsigned char **text_line = text;
	int x, char_x, y, char_y, i, j;
	unsigned char *line_pos;
	unsigned char **char_bitmap;
	
	while (*text_line != NULL) {
		height += 16;
		width = line_width(*text_line);
		if (width > max_width) max_width = width;
		text_line++;
	}
	
	b = malloc(sizeof(bitmap));
	b->width = max_width;
	b->height = height;
	b->pixels = calloc(b->width * b->height, sizeof(unsigned char));
	b->mask = calloc(b->width * b->height, sizeof(unsigned char));
	memset(b->mask, 0xff, b->width * b->height);
	
	text_line = text;
	char_y = 0;

	while(*text_line != NULL) {
		/* for each line */
		char_x = (max_width - line_width(*text_line)) / 2;
		line_pos = *text_line;
		while (*line_pos != '\0') {
			/* for each char */
			y = char_y;
			char_bitmap = aluminium_font[*line_pos];
			for (i = 0; i < 16; i++) {
				/* for each pixel row */
				x = char_x;
				for (j = 0; char_bitmap[i][j] != '\0'; j++) {
					/* for each pixel */
					switch (char_bitmap[i][j]) {
						case ' ': /* black */
							b->mask[x+y] = 0;
							b->pixels[x+y] = 3;
							break;
						case 'X': /* dkgrey */
							b->mask[x+y] = 0;
							b->pixels[x+y] = 2;
							break;
						case 'o': /* ltgrey */
							b->mask[x+y] = 0;
							b->pixels[x+y] = 1;
							break;
						case 'O': /* white */
							b->mask[x+y] = 0;
							break;
						/* remaining case is background, where mask/pixel values are left as default */
					}
					x++;
				}
				y += max_width;
			}
			char_x += strlen(char_bitmap[0]);
			line_pos++;
		}
		text_line++;
		char_y += 16 * max_width;
	}
	
	return b;
}
