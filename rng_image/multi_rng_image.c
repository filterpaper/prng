/* Copyright (C) 2021 @filterpaper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* This is a simple pseudo random number generator tester using bitmap
   image pixels. Good pseudo random generator will produce an image
   like white noise, and bad output will contain discernible patterns.

   Bitmap file output settings can be configured with preprocessor
   macros the follows. Pick one pseudo random generator function with
   the RNG macro.

   To test any pseudo random number generator function, add them above
   main() function, and link it to the macro RND.

   This file automaticaly generates bmp images for all rng function,
   each RNG return value is type cast as 64-bit for consistency
   as pointer variable to generate_image().
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Image output settings
#define GRAYSCALE // Exclude for full RGB
#define WIDTH  640
#define HEIGHT 1200



// pcg_mcg_16_xsh_rr_8_random_r
static uint_fast64_t pcg8(void) {
	static uint16_t state = 0x6835;

	uint16_t oldstate = state;
	state = state * 12829U;

	uint8_t value = ((oldstate >> 5U) ^ oldstate) >> 5U;
	uint32_t rot = oldstate >> 13U;
	return (value >> rot) | (value << ((- rot) & 7));
}

// pcg_mcg_16_xsh_rs_8_random_r
static uint_fast64_t pcg8_fast(void) {
	static uint16_t state = 0xc207;

	uint16_t oldstate = state;
	state = state * 12829U + 1U;

	return ((oldstate >> 7U) ^ oldstate) >> ((oldstate >> 14U) + 3U);
}


// pcg_mcg_32_xsh_rr_16_random_r
static uint_fast64_t pcg16(void) {
	static uint32_t state = 0x406832dd;

	uint32_t oldstate = state;
	state = state * 747796405U + 1U;

	uint16_t value = ((oldstate >> 10U) ^ oldstate) >> 12U;
	uint32_t rot = oldstate >> 28U;
	return (value >> rot) | (value << ((- rot) & 15));
}

// pcg_mcg_32_xsh_rs_16_random_r
static uint_fast64_t pcg16_fast(void) {
	static uint32_t state = 0x406832dd;

	uint32_t oldstate = state;
	state = state * 747796405U + 1U;

	return ((oldstate >> 11U) ^ oldstate) >> ((oldstate >> 30U) + 11U);
}


// pcg_mcg_64_xsh_rr_32_random_r
static uint_fast64_t pcg32(void) {
	static uint64_t state = 0x406832dd910219e5;

	uint64_t oldstate = state;
	state = state * 6364136223846793005ULL;

	uint32_t value = ((oldstate >> 18U) ^ oldstate) >> 27U;
	uint32_t rot = oldstate >> 59U;
	return (value >> rot) | (value << ((- rot) & 31));
}

// pcg_mcg_64_xsh_rs_32_random_r
static uint_fast64_t pcg32_fast(void) {
	static uint64_t state = 0x406832dd910219e5;

	uint64_t oldstate = state;
	state = state * 6364136223846793005ULL;

	return ((oldstate >> 22U) ^ oldstate) >> ((oldstate >> 61U) + 22U);
}



// Brad Forschinger's 16 bit xorshift rng
// http://b2d-f9r.blogspot.com/2010/08/16-bit-xorshift-rng-now-with-more.html
static uint_fast64_t rnd_xorshift_16(void) {
	static uint_fast16_t x = 1, y = 1;
	uint_fast16_t t = (x ^ (x << 5));
	x = y;
	return (uint_fast64_t)(y = (y ^ (y >> 1)) ^ (t ^ (t >> 3)));
}



// George Marsaglia's xorshift32
// https://en.wikipedia.org/wiki/Xorshift
static uint_fast64_t xorshift32(void) {
	// Seed this 32 bit manually
	static uint_fast32_t a = 0x2b2b0c5a;

	uint_fast32_t x = a;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return (uint_fast64_t)(a = x);
}

// George Marsaglia's xorshift64
// https://en.wikipedia.org/wiki/Xorshift
static uint_fast64_t xorshift64(void) {
	// Seed this 64 bit manually
	static uint_fast64_t a = 0x0b7195eb5263650b;

	uint_fast64_t x = a;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return a = x;
}

// George Marsaglia's xorshift128
// https://en.wikipedia.org/wiki/Xorshift
static uint_fast64_t xorshift128(void) {
	// Seed these 32 bit manually
	static uint_fast32_t a = 0x16e12e4b;
	static uint_fast32_t b = 0xe4d4a624;
	static uint_fast32_t c = 0x80ccff3c;
	static uint_fast32_t d = 0x3b9b979f;

	uint_fast32_t t = d;
	uint_fast32_t const s = a;
	d = c; c = b; b = s;

	t ^= t << 11;
	t ^= t >> 8;
	return (uint_fast64_t)(a = t ^ s ^ (s >> 19));
}



// General-purpose xoshiro 64-bit PRNG variants
// https://en.wikipedia.org/wiki/Xorshift
static uint_fast64_t rol64(uint64_t x, int k) {
	return (x << k) | (x >> (64 - k));
}

// xoshiro256**
static uint_fast64_t xoshiro256ss(void) {
	// Seed these 64 bit manually
	static uint_fast64_t s0 = 0x1e872bf277bbd5e4;
	static uint_fast64_t s1 = 0xba317487d87c4159;
	static uint_fast64_t s2 = 0x362fa9620e9a1c8a;
	static uint_fast64_t s3 = 0xc918db525116e49c;

	uint_fast64_t const result = rol64(s1 * 5, 7) * 9;
	uint_fast64_t const t = s1 << 17;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;
	s3 = rol64(s3, 45);

	return result;
}

// xoshiro256+ faster variant
static uint_fast64_t xoshiro256p(void) {
	// Seed these 64 bit manually
	static uint_fast64_t s0 = 0x300eb059795dc07c;
	static uint_fast64_t s1 = 0x5b80e5ea8841a0cf;
	static uint_fast64_t s2 = 0x919572a617c344d3;
	static uint_fast64_t s3 = 0xa92e3913be8d584d;

	uint_fast64_t const result = s0 + s3;
	uint_fast64_t const t = s1 << 17;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;
	s3 = rol64(s3, 45);

	return result;
}



// xoroshiro 64-bit PRNG variants
// Utilizes the same xoshiro rotate function
// https://prng.di.unimi.it/
// xoroshiro128+
static uint_fast64_t xoroshiro128p(void) {
	// Seed these 64 bit manually
	static uint_fast64_t s0 = 0x305e0bc4831f6240;
	static uint_fast64_t s1 = 0x7cff19974aef796d;

	uint_fast64_t const t0 = s0;
	uint_fast64_t t1 = s1;
	uint_fast64_t const result = t0 + t1;

	t1 ^= t0;
	s0 = rol64(t0, 24) ^ t1 ^ (t1 << 16); // a, b
	s1 = rol64(t1, 37); // c

	return result;
}

// xoroshiro128++
static uint_fast64_t xoroshiro128pp(void) {
	// Seed these 64 bit manually
	static uint_fast64_t s0 = 0xaafdbd4fce743b4d;
	static uint_fast64_t s1 = 0xcaee5c952c4ae6a8;

	uint_fast64_t const t0 = s0;
	uint_fast64_t t1 = s1;
	uint_fast64_t const result = rol64(t0 + t1, 17) + t0;

	t1 ^= t0;
	s0 = rol64(t0, 49) ^ t1 ^ (t1 << 21); // a, b
	s1 = rol64(t1, 28); // c

	return result;
}

// xoroshiro128**
static uint_fast64_t xoroshiro128ss(void) {
	// Seed these 64 bit manually
	static uint_fast64_t s0 = 0xa838cd0e0aeb110b;
	static uint_fast64_t s1 = 0x99fe19b209da8e32;

	uint_fast64_t const t0 = s0;
	uint_fast64_t t1 = s1;
	uint_fast64_t const result = rol64(t0 * 5, 7) * 9;

	t1 ^= t0;
	s0 = rol64(t0, 24) ^ t1 ^ (t1 << 16); // a, b
	s1 = rol64(t1, 37); // c

	return result;
}



// @tzarc's 8 bit XORshift, produces minor repeated pattern
// https://github.com/tzarc/qmk_build/blob/bebe5e5b21e99bdb8ff41500ade1eac2d8417d8c/users-tzarc/tzarc_common.c#L57-L63
static uint_fast64_t tzarc_prng(void) {
	static uint8_t s = 0xAA, a = 0;
	s ^= s << 3;
	s ^= s >> 5;
	s ^= a++ >> 2;
	return (uint_fast64_t)s;
}



// Simple 4-register 8 bit XORshift
// https://github.com/edrosten/8bit_rng
static uint_fast64_t xshift8(void) {
	static uint_fast8_t x = 0, y = 0, z = 0, a = 1;
	uint_fast8_t t = x ^ (x << 5);
	x = y; y = z; z = a;
	return a = z ^ ( z >> 1) ^ t ^ (t << 3);
}



// Bob Jenkins PRNG in 32-bit
// http://burtleburtle.net/bob/rand/smallprng.html
// jsf = Jenkins Small Fast
uint_fast64_t jsf64(void) {
	static uint64_t a = 0xf1eaeb0597955eed;
	static uint64_t b = 0x80ccff3cee5c952c, c = 0x80ccff3cee5c952c, d = 0x80ccff3cee5c952c;

	uint64_t e = a - ((b << 7)|(b >> 57));
	a = b ^ ((c << 13)|(c >> 51));
	b = c + ((d << 37)|(d >> 27));;
	c = d + e;
	d = e + a;
	return d;
}
uint_fast64_t jsf32(void) {
	static uint32_t a = 0xf1ea5eed;
	static uint32_t b = 0x80ccff3c, c = 0x80ccff3c, d = 0x80ccff3c;

	uint32_t e = a - ((b << 27)|(b >> 5));
	a = b ^ ((c << 17)|(c >> 15));
	b = c + d;
	c = d + e;
	d = e + a;
	return d;
}
// Bob Jenkins PRNG in 16-bit
uint_fast64_t jsf16(void) {
	static uint16_t a = 0xf1ea;
	static uint16_t b = 0x80cc, c = 0x80cc, d = 0x80cc;

	uint16_t e = a - ((b << 13)|(b >> 3));
	a = b ^ ((c << 8)|(c >> 8));
	b = c + d;
	c = d + e;
	d = e + a;
	return d;
}
// Bob Jenkins PRNG in 8-bit
uint_fast64_t jsf8(void) {
	static uint8_t a = 0xf1;
	static uint8_t b = 0xee, c = 0xee, d = 0xee;

	uint8_t e = a - ((b << 1)|(b >> 7));
	a = b ^ ((c << 4)|(c >> 4));
	b = c + d;
	c = d + e;
	d = e + a;
	return d;
}



// C library's rand()
static uint_fast64_t crand(void) {
	return (uint_fast64_t)rand();
}



// Image loop that generates randomized pixel image into 32bit.bmp
// https://stackoverflow.com/questions/50090500/create-simple-bitmap-in-c-without-external-libraries
void generate_image(uint_fast64_t (*rngfunction)(), char filename[]) {
	//width, height, and bitcount are the key factors:
	static uint_fast32_t const width  = WIDTH;
	static uint_fast32_t const height = HEIGHT;
	static uint_fast16_t const bitcount = 24;//<- 24-bit bitmap

	//take padding in to account
	int width_in_bytes = ((width * bitcount + 31) / 32) * 4;

	//total image size in bytes, not including header
	uint_fast32_t imagesize = width_in_bytes * height;

	//this value is always 40, it's the sizeof(BITMAPINFOHEADER)
	static uint_fast32_t const biSize = 40;

	//bitmap bits start after headerfile,
	//this is sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
	static uint_fast32_t const bfOffBits = 54;

	//total file size:
	uint_fast32_t filesize = 54 + imagesize;

	//number of planes is usually 1
	static uint_fast16_t const biPlanes = 1;

	//create header:
	//copy to buffer instead of BITMAPFILEHEADER and BITMAPINFOHEADER
	//to avoid problems with structure packing
	unsigned char header[54] = { 0 };
	memcpy(header, "BM", 2);
	memcpy(header + 2 , &filesize, 4);
	memcpy(header + 10, &bfOffBits, 4);
	memcpy(header + 14, &biSize, 4);
	memcpy(header + 18, &width, 4);
	memcpy(header + 22, &height, 4);
	memcpy(header + 26, &biPlanes, 2);
	memcpy(header + 28, &bitcount, 2);
	memcpy(header + 34, &imagesize, 4);

	//prepare pixel data:
	unsigned char* buf = malloc(imagesize);
	for(int row = height - 1; row >= 0; row--) {
		for(int col = 0; col < width; col++) {

			uint_fast8_t blue  = (uint8_t)rngfunction();
			uint_fast8_t green = (uint8_t)rngfunction();
			uint_fast8_t red   = (uint8_t)rngfunction();

#ifdef GRAYSCALE
			uint_fast8_t gray = 0.3*red + 0.59*green + 0.11*blue;
			blue = green = red = gray;
#endif
			buf[row * width_in_bytes + col * 3 + 0] = blue;
			buf[row * width_in_bytes + col * 3 + 1] = green;
			buf[row * width_in_bytes + col * 3 + 2] = red;
		}
	}

	FILE *fout = fopen(filename, "wb");
	fwrite(header, 1, 54, fout);
	fwrite((char*)buf, 1, imagesize, fout);
	fclose(fout);
	free(buf);

	return;
}


int main(void) {

	generate_image(&pcg8, "pcg8.bmp");
	generate_image(&pcg8_fast, "pcg8_fast.bmp");
	generate_image(&pcg16, "pcg16.bmp");
	generate_image(&pcg16_fast, "pcg16_fast.bmp");
	generate_image(&pcg32, "pcg32.bmp");
	generate_image(&pcg32_fast, "pcg32_fast.bmp");
	generate_image(&rnd_xorshift_16, "rnd_xorshift_16.bmp");
	generate_image(&xorshift32, "xorshift32.bmp");
	generate_image(&xorshift32, "xorshift64.bmp");
	generate_image(&xorshift128, "xorshift128.bmp");
	generate_image(&xoshiro256ss, "xoshiro256ss.bmp");
	generate_image(&xoshiro256p, "xoshiro256p.bmp");
	generate_image(&xoroshiro128p, "xoroshiro128p.bmp");
	generate_image(&xoroshiro128pp, "xoroshiro128pp.bmp");
	generate_image(&xoroshiro128ss, "xoroshiro128ss.bmp");
	generate_image(&tzarc_prng, "tzarc_prng.bmp");
	generate_image(&xshift8, "xshift8.bmp");
	generate_image(&jsf64, "jsf64.bmp");
	generate_image(&jsf32, "jsf32.bmp");
	generate_image(&jsf16, "jsf16.bmp");
	generate_image(&jsf8, "jsf8.bmp");
	generate_image(&crand, "crand.bmp");

	// RNG number sampling
//	for (int i = 100; i > 0; --i) { printf("rnd: 0x%16llx\n", xorshift128()); }

	return 0;
}
