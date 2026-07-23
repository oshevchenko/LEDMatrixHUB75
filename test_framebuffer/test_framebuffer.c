#include "framebuffer.h"
void testimage_set(unsigned int x, unsigned int y, uint16_t r, uint16_t g, uint16_t b) {
	unsigned int offset;
	if (y >=32 && y < 64) {
		y+=32;
	} else if (y >= 64 && y < 96) {
		y-=32;
	}
	offset=(x + (y*MATRIX_WIDTH))*3;
	framebuffer_write(offset, r);
	framebuffer_write(offset+1, g);
	framebuffer_write(offset+2, b);
}
void testimage_set_test(unsigned int x, unsigned int y, uint16_t r, uint16_t g, uint16_t b) {
	unsigned int offset;
	if (y >=32 && y < 64) {
		y+=32;
	} else if (y >= 64 && y < 96) {
		y-=32;
	}
	offset=x + (y*MATRIX_WIDTH);
	test_framebuffer_write(offset, r, g, b);
}

int main() {
    int mismatch_found = 0;
    framebuffer_init();
	mismatch_found = compare_framebuffers();
	if (mismatch_found) {
		// printf("Mismatch found at x=%u, y=%u\n", x, y);
		return 1;
	}
    for (unsigned int x = 0; x < MATRIX_WIDTH; x++) {
        for (unsigned int y = 0; y < MATRIX_HEIGHT; y++) {
			// get 3 random values for r, g, b
			uint16_t r = 0;
			uint16_t g = 0;
			uint16_t b = 0;
            testimage_set(x, y, r, g, b); // 
            testimage_set_test(x, y, r, g, b); // 
        }
    }
	mismatch_found = compare_framebuffers();
	if (mismatch_found) {
		return 1;
	}

    for (unsigned int x = 0; x < MATRIX_WIDTH; x++) {
        for (unsigned int y = 0; y < MATRIX_HEIGHT; y++) {
			// get 3 random values for r, g, b
			uint16_t r = (x * 31) % 65536; // Example pattern
			uint16_t g = (y * 63) % 65536; // Example pattern
			uint16_t b = ((x + y) * 127) % 65536;
            testimage_set(x, y, r, g, b); // 
            testimage_set_test(x, y, r, g, b); // 
        }
		for (unsigned int x = 0; x < MATRIX_WIDTH; x++) {
			for (unsigned int y = 0; y < MATRIX_HEIGHT; y++) {
				// get 3 random values for r, g, b
				uint16_t r = 0;
				uint16_t g = 0;
				uint16_t b = 0;
				testimage_set(x, y, r, g, b); // 
				testimage_set_test(x, y, r, g, b); // 
			}
		}
		mismatch_found = compare_framebuffers();
		if (mismatch_found) {
			return 1;
		}
    }
	mismatch_found = compare_framebuffers();
	if (mismatch_found) {
		return 1;
	}
	framebuffer_swap();
	framebuffer_swap();
	framebuffer_swap();
	framebuffer_swap();
    return 0;
}