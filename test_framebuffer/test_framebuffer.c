#include "framebuffer.h"
void testimage_set(unsigned int x, unsigned int y, uint8_t r, uint8_t g, uint8_t b) {
	unsigned int offset;
	if (y >=32 && y < 64) {
		y+=32;
	} else if (y >= 64 && y < 96) {
		y-=32;
	}
	offset=(x + (y*MATRIX_WIDTH))*3;
	framebuffer_write(offset,0);
	framebuffer_write(offset+1,0);
	framebuffer_write(offset+2,0);
}

int main() {
    framebuffer_init();
    for (unsigned int x = 0; x < MATRIX_WIDTH; x++) {
        for (unsigned int y = 0; y < MATRIX_HEIGHT; y++) {
            testimage_set(x, y, 255, 0, 0); // Set all pixels to red
        }
    }
    return 0;
}