/*
 * framebuffer.c
 *
 *  Created on: 11 dec. 2014
 *      Author: Frans-Willem
 */


#include "framebuffer.h"
#include "offset_table.h"
//Current setup:
//For each bit
//	for each row
//		data to be clocked out
//New setup:
//For each row
//	for each bit
//		data to be clocked out

//Number of elements per row
//Number of elements per bit (e.g. a row for each scanrow)
#define FRAMEBUFFER_BITLEN	FRAMEBUFFER_SHIFTLEN
#define FRAMEBUFFER_ROWLEN	(FRAMEBUFFER_BITLEN * FRAMEBUFFER_MAXBITDEPTH)
//Length of one framebuffer
#define FRAMEBUFFER_LEN		(FRAMEBUFFER_ROWLEN * MATRIX_PANEL_SCANROWS)

FRAMEBUFFER_TYPE framebuffers[FRAMEBUFFER_LEN*FRAMEBUFFER_BUFFERS]={0};
volatile unsigned int framebuffer_writebuffer=0;
volatile unsigned int framebuffer_displaybuffer=0;

void framebuffer_init() {
	unsigned int i;
	for (i=0; i<FRAMEBUFFER_LEN*FRAMEBUFFER_BUFFERS; i++) {
		framebuffers[i]=(i&1)?FRAMEBUFFER_CLOCK:0;
	}
	framebuffer_displaybuffer=0;
	framebuffer_writebuffer=(framebuffer_displaybuffer+1)%FRAMEBUFFER_BUFFERS;
}

void framebuffer_clean() {
	unsigned int i;
	FRAMEBUFFER_TYPE *ptr = &framebuffers[(framebuffer_writebuffer * FRAMEBUFFER_LEN)];
	for (i=0; i<FRAMEBUFFER_LEN; i++) {
		ptr[i]=(i&1)?FRAMEBUFFER_CLOCK:0;
	}
}

void framebuffer_write_table(unsigned int offset, uint16_t value_r, uint16_t value_g, uint16_t value_b) {
	FRAMEBUFFER_TYPE output_r, output_g, output_b;
	if (offset < 3072) {
		output_r = 1;
		output_g = 2;
		output_b = 4;
	} else if (offset < 6144) {
		output_r = 8;
		output_g = 16;
		output_b = 32;
	} else if (offset < 9216) {
		output_r = 1;
		output_g = 2;
		output_b = 4;
	} else {
		output_r = 8;
		output_g = 16;
		output_b = 32;
	}
	const unsigned int *ptr_flash = &flash_offset_table[0];
	ptr_flash += offset;
	unsigned int new_offset = *ptr_flash;
	if (new_offset >= FRAMEBUFFER_LEN){
		return;
	}
	FRAMEBUFFER_TYPE *ptr = &framebuffers[(framebuffer_writebuffer * FRAMEBUFFER_LEN)+new_offset];
	// printf("Pointer offset:0x%X\n", (framebuffer_writebuffer * FRAMEBUFFER_LEN)+offset);
	unsigned int bit;
	for (bit = (1 << (FRAMEBUFFER_MAXBITDEPTH-1)); bit; bit>>=1) {
		if (value_r & bit) {
			ptr[0]|=output_r;
			ptr[1]|=output_r;
		} else {
			ptr[0]&=~output_r;
			ptr[1]&=~output_r;
		}
		if (value_g & bit) {
			ptr[0]|=output_g;
			ptr[1]|=output_g;
		} else {
			ptr[0]&=~output_g;
			ptr[1]&=~output_g;
		}
		if (value_b & bit) {
			ptr[0]|=output_b;
			ptr[1]|=output_b;
		} else {
			ptr[0]&=~output_b;
			ptr[1]&=~output_b;
		}
		ptr = &ptr[FRAMEBUFFER_BITLEN];
	}

}
void framebuffer_write(unsigned int offset, uint16_t value) {
	unsigned int channel = offset % MATRIX_PANEL_CHANNELS; offset/=MATRIX_PANEL_CHANNELS;
	unsigned int x = offset % MATRIX_WIDTH; offset/=MATRIX_WIDTH;
	unsigned int y = offset;
	if (y >= MATRIX_HEIGHT)
		return;
	unsigned int segment = y / MATRIX_PANEL_HEIGHT;
	unsigned int scanrow = y % MATRIX_PANEL_SCANROWS;
	unsigned int bus = (y / MATRIX_PANEL_SCANROWS) % MATRIX_PANEL_BUSES;
	//Every odd segment (line of panels) is inverted (snake-like configuration)
	// if (segment & 1) {
	// 	x = (MATRIX_WIDTH-1)-x;
	// 	scanrow = (MATRIX_PANEL_SCANROWS-1)-scanrow;
	// 	bus = (MATRIX_PANEL_BUSES-1)-bus;
	// }
	offset = (scanrow * FRAMEBUFFER_ROWLEN) + ((MATRIX_PANELSH-1-segment) * MATRIX_PANELSW * MATRIX_PANEL_WIDTH * 2) + (x * 2);
	FRAMEBUFFER_TYPE output = 1<<((bus*MATRIX_PANEL_CHANNELS) + channel);
	FRAMEBUFFER_TYPE *ptr = &framebuffers[(framebuffer_writebuffer * FRAMEBUFFER_LEN)+offset];
	unsigned int bit;
	for (bit = (1 << (FRAMEBUFFER_MAXBITDEPTH-1)); bit; bit>>=1) {
		if (value & bit) {
			ptr[0]|=output;
			ptr[1]|=output;
		} else {
			ptr[0]&=~output;
			ptr[1]&=~output;
		}
		ptr = &ptr[FRAMEBUFFER_BITLEN];
	}
}
FRAMEBUFFER_TYPE *framebuffer_get() {
	return &framebuffers[framebuffer_displaybuffer * FRAMEBUFFER_LEN];
}

void framebuffer_swap() {
	framebuffer_displaybuffer=framebuffer_writebuffer;
	framebuffer_writebuffer=(framebuffer_displaybuffer+1)%FRAMEBUFFER_BUFFERS;
}

void framebuffer_sync() {
	//TODO: Implement waiting for DMA to actually display framebuffer
}
