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
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Image output settings
//#define BIGSAMPLE // RNG on each color
#define WIDTH  800
#define HEIGHT 800
#define FILENAME "24bit.bmp"


// RNG function selection, pick one
#define RND pcg8()
//#define RND pcg8_fast()
//#define RND pcg16()
//#define RND pcg16_fast()
//#define RND pcg32()
//#define RND pcg32_fast()
//#define RND xorshift32()
//#define RND xorshift64()
//#define RND xorshift128()
//#define RND xoshiro256ss()
//#define RND xoshiro256p()
//#define RND xoroshiro128p()
//#define RND xoroshiro128pp()
//#define RND xoroshiro128ss()
//#define RND brad16()
//#define RND tzarc_prng()
//#define RND xshift8()
//#define RND jsf64()
//#define RND jsf32()
//#define RND jsf16()
//#define RND jsf8()


// pcg_mcg_16_xsh_rr_8_random_r
uint8_t pcg8(void) {
	static uint16_t state = 0x2af3;
	static uint16_t const inc = 0x3c97;

	uint16_t oldstate = state;
	state = state * 12829U + (inc|1);

	#define pcg_rot8(x,k) (((x) >> (k))|((x) << ((- k) & 7)))
	return pcg_rot8((uint8_t)(((oldstate >> 5U) ^ oldstate) >> 5U), (oldstate >> 13U));
}

// pcg_mcg_16_xsh_rs_8_random_r
uint8_t pcg8_fast(void) {
	static uint16_t state = 0xc207;

	uint16_t oldstate = state;
	state = state * 12829U + 1U;

	return ((oldstate >> 7U) ^ oldstate) >> ((oldstate >> 14U) + 3U);
}


// pcg_mcg_32_xsh_rr_16_random_r
static uint16_t pcg16(void) {
	static uint32_t state = 0x406832dd;

	uint32_t oldstate = state;
	state = state * 747796405U + 1U;

/*	uint16_t value = ((oldstate >> 10U) ^ oldstate) >> 12U;
	uint32_t rot = oldstate >> 28U;
	return (value >> rot) | (value << ((- rot) & 15));*/
	#define pcg_rot16(x,k) (((x) >> (k))|((x) << ((- k) & 15)))
	return pcg_rot16((uint16_t)(((oldstate >> 10U) ^ oldstate) >> 12U), (oldstate >> 28U));
}

// pcg_mcg_32_xsh_rs_16_random_r
static uint16_t pcg16_fast(void) {
	static uint32_t state = 0x406832dd;

	uint32_t oldstate = state;
	state = state * 747796405U + 1U;

	return ((oldstate >> 11U) ^ oldstate) >> ((oldstate >> 30U) + 11U);
}


// pcg_mcg_64_xsh_rr_32_random_r
static uint32_t pcg32(void) {
	static uint64_t state = 0x406832dd910219e5;

	uint64_t oldstate = state;
	state = state * 6364136223846793005ULL;

	uint32_t value = ((oldstate >> 18U) ^ oldstate) >> 27U;
	uint32_t rot = oldstate >> 59U;
	return (value >> rot) | (value << ((- rot) & 31));
}

// pcg_mcg_64_xsh_rs_32_random_r
static uint32_t pcg32_fast(void) {
	static uint64_t state = 0x406832dd910219e5;

	uint64_t oldstate = state;
	state = state * 6364136223846793005ULL;

	return ((oldstate >> 22U) ^ oldstate) >> ((oldstate >> 61U) + 22U);
}




// George Marsaglia's xorshift32
// https://en.wikipedia.org/wiki/Xorshift
static uint_fast32_t xorshift32(void) {
	// Seed this 32 bit manually
	static uint_fast32_t a = 0x2b2b0c5a;

	uint_fast32_t x = a;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return a = x;
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
static uint_fast32_t xorshift128(void) {
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
	return a = t ^ s ^ (s >> 19);
}



// General-purpose xoshiro 64-bit PRNG variants
// https://en.wikipedia.org/wiki/Xorshift
static uint_fast64_t rol64(uint_fast64_t const x, int const k) {
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


// Brad Forschinger's 16 bit xorshift rng
// http://b2d-f9r.blogspot.com/2010/08/16-bit-xorshift-rng-now-with-more.html
uint16_t brad16(void) {
	static uint8_t const a = 5, b = 3, c = 13;
	static uint16_t x = 1, y = 1;
	uint16_t t = (x ^ (x << a));
	x = y * 3;
	return y = (y ^ (y >> c)) ^ (t ^ (t >> b));
}


// @tzarc's 8 bit XORshift, produces minor repeated pattern
// https://github.com/tzarc/qmk_build/blob/bebe5e5b21e99bdb8ff41500ade1eac2d8417d8c/users-tzarc/tzarc_common.c#L57-L63
static uint8_t tzarc_prng(void) {
	static uint8_t s = 0xAA, a = 0;
	s ^= s << 3;
	s ^= s >> 5;
	s ^= a++ >> 2;
	return s;
}



// Simple 4-register 8 bit XORshift
// https://github.com/edrosten/8bit_rng
static uint_fast8_t xshift8(void) {
	static uint_fast8_t x = 0, y = 0, z = 0, a = 1;
	uint_fast8_t t = x ^ (x << 5);
	x = y; y = z; z = a;
	return a = z ^ ( z >> 1) ^ t ^ (t << 3);
}



// http://burtleburtle.net/bob/rand/smallprng.html
// jsf = Jenkins Small Fast
// Bob Jenkins PRNG in 64-bit
#define rot64(x,k) ((x << k)|(x >> (64 - k)))
uint64_t jsf64(void) {
	static uint64_t a = 0xf1eaeb0597955eed;
	static uint64_t b = 0x80ccff3cee5c952c, c = 0x80ccff3cee5c952c, d = 0x80ccff3cee5c952c;

	uint64_t e = a - rot64(b, 7);
	a = b ^ rot64(c, 13);
	b = c + rot64(d, 37);
	c = d + e;
	return d = e + a;
}
// Bob Jenkins PRNG in 32-bit
#define rot32(x,k) ((x << k)|(x >> (32 - k)))
uint32_t jsf32(void) {
	static uint32_t a = 0xf1ea5eed;
	static uint32_t b = 0x80ccff3c, c = 0x80ccff3c, d = 0x80ccff3c;

	uint32_t e = a - rot32(b, 27);
	a = b ^ rot32(c, 17);
	b = c + d;
	c = d + e;
	return d = e + a;
}
// Bob Jenkins PRNG in 16-bit
#define rot16(x,k) ((x << k)|(x >> (16 - k)))
uint16_t jsf16(void) {
	static uint16_t a = 0xf1ea;
	static uint16_t b = 0x80cc, c = 0x80cc, d = 0x80cc;

	uint16_t e = a - rot16(b, 13);
	a = b ^ rot16(c, 8);
	b = c + d;
	c = d + e;
	return d = e + a;
}
// Bob Jenkins PRNG in 8-bit
#define rot8(x,k) ((x << k)|(x >> (8 - k)))
uint8_t jsf8(void) {
	static uint8_t a = 0xf1;
	static uint8_t b = 0xee, c = 0xee, d = 0xee;

	uint8_t e = a - rot8(b, 1);
	a = b ^ rot8(c, 4);
	b = c + d;
	c = d + e;
	return d = e + a;
}




// Main loop that generates randomized pixel image into 32bit.bmp
// https://stackoverflow.com/questions/50090500/create-simple-bitmap-in-c-without-external-libraries
int main(void) {
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
#ifndef BIGSAMPLE
			uint_fast8_t blue, green, red;
			blue = green = red = (uint8_t)RND;
#else
			uint_fast8_t blue  = (uint8_t)RND;
			uint_fast8_t green = (uint8_t)RND;
			uint_fast8_t red   = (uint8_t)RND;

			uint_fast8_t gray = 0.3*red + 0.59*green + 0.11*blue;
			//uint_fast8_t gray = (red + green + blue)/3;
			blue = green = red = gray;
#endif
			buf[row * width_in_bytes + col * 3 + 0] = blue;
			buf[row * width_in_bytes + col * 3 + 1] = green;
			buf[row * width_in_bytes + col * 3 + 2] = red;
		}
	}

	FILE *fout = fopen(FILENAME, "wb");
	fwrite(header, 1, 54, fout);
	fwrite((char*)buf, 1, imagesize, fout);
	fclose(fout);
	free(buf);

	// RNG number sampling
//	for (int i = 100; i > 0; --i) { printf("rnd: 0x%16llx\n", RND); }

	return 0;
}
