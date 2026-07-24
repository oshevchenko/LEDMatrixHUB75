/*
 * testimage.c
 *
 *  Created on: 12 dec. 2014
 *      Author: Frans-Willem
 */

#include "framebuffer.h"
#include "colorcorr.h"
// #define OLD_TESTIMAGE_SET
#ifdef OLD_TESTIMAGE_SET

void testimage_set(unsigned int x, unsigned int y, uint8_t r, uint8_t g, uint8_t b) {
	unsigned int offset;
	if (y >=32 && y < 64) {
		y+=32;
	} else if (y >= 64 && y < 96) {
		y-=32;
	}
	offset=(x + (y*MATRIX_WIDTH))*3;
	framebuffer_write(offset,colorcorr_lookup(r));
	framebuffer_write(offset+1,colorcorr_lookup(b));
	framebuffer_write(offset+2,colorcorr_lookup(g));
}

void testimage_setb(unsigned int x, unsigned int y, uint8_t* rgb) {
	unsigned int offset;
	if (y >=32 && y < 64) {
		y+=32;
	} else if (y >= 64 && y < 96) {
		y-=32;
	}
	offset=(x + (y*MATRIX_WIDTH))*3;
	framebuffer_write(offset,colorcorr_lookup(rgb[0]));
	framebuffer_write(offset+1,colorcorr_lookup(rgb[2]));
	framebuffer_write(offset+2,colorcorr_lookup(rgb[1]));
}
#else
void testimage_set(unsigned int x, unsigned int y, uint8_t r, uint8_t g, uint8_t b) {
	unsigned int offset;
	offset=x + (y*MATRIX_WIDTH);
	framebuffer_write_96x128x5(offset,colorcorr_lookup(r), colorcorr_lookup(g), colorcorr_lookup(b));
}

void testimage_setb(unsigned int x, unsigned int y, uint8_t* rgb) {
	unsigned int offset;
	offset=x + (y*MATRIX_WIDTH);
	framebuffer_write_96x128x5(offset,colorcorr_lookup(rgb[0]), colorcorr_lookup(rgb[1]), colorcorr_lookup(rgb[2]));
}

#endif
#define MIN(a,b) (((a)<(b))?(a):(b))
uint8_t topcolors[]={
	0,   0, 255,
	0, 255,   0,
	0, 255, 255,
	255, 0,   0,
	255, 0, 255,
	255,255,  0,
	255,255,255,
};
uint8_t barcolors[]={
	0,0,255,
	0,0,0,
	255,0,255,
	0,0,0,
	0,255,255,
	0,0,0,
	255,255,255
};

void testimage_init() {
	unsigned int x,y;
	for (x=0; x<MATRIX_WIDTH; x++) {
		for (y=0; y<MATRIX_HEIGHT; y++) {
			unsigned int seg = (y * 13)/MATRIX_HEIGHT;
			if (seg < 8) {
				unsigned int c=(x * ((sizeof(topcolors)/sizeof(topcolors[0]))/3))/MATRIX_WIDTH;
				testimage_setb(x,y,&topcolors[c*3]);
			} else if (seg == 8) {
				unsigned int c=(x * ((sizeof(barcolors)/sizeof(barcolors[0]))/3))/MATRIX_WIDTH;
				testimage_setb(x,y,&barcolors[c*3]);
			} else {
				unsigned int v = (x * 255)/MATRIX_WIDTH;
				unsigned int c = seg-9;
				testimage_set(x,y,(c == 0 || c == 3)?v:0,(c == 1 || c == 3)?v:0,(c == 2 || c == 3)?v:0);
			}
		}
	}
	framebuffer_swap();
}

void testimage_run() {
	static unsigned int x_white=0;
	static unsigned int y_white=0;
	static unsigned int y_treshold=0;
	static unsigned int j=0;
	unsigned int x,y;

	framebuffer_clean();
	for (y=0; y<MATRIX_HEIGHT; y++) {
		for (x=0; x<MATRIX_WIDTH; x++) {
			if (y == y_white) {
				// testimage_set(x,y,255,255,255);
				testimage_setb(x,y,&topcolors[j*3]);
			} else {
				// testimage_set(x,y,0,0,0);
			}
		}
	}
	y_white+=1;
	if (y_white >= MATRIX_HEIGHT) {
		y_white=0;
		j++;
		if (j >= (sizeof(topcolors)/sizeof(topcolors[0]))/3) {
			j=0;
		}
	}
	framebuffer_swap();
}
