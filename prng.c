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

/* This is a repository of various pseudorandom number generator functions
   with states that are seeded manually.

   Main function will output continuous stream of selected PRNG selected
   with command line argument.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


// PCG family
// https://www.pcg-random.org/download.html
// pcg_mcg_16_xsh_rr_8_random_r
uint8_t pcg8(void) {
	static uint16_t state = 0x9af1;
	static uint16_t const inc = 0x0527;

	uint16_t x = state;
	state = state * 12829U + (inc|1);

	uint16_t value = ((x >> 5U) ^ x) >> 5U;
	uint32_t rot = x >> 13U;
	return (value >> rot) | (value << ((- rot) & 7));
}
// pcg_mcg_16_xsh_rs_8_random_r
uint8_t pcg8_fast(void) {
	static uint16_t state = 0x1091;

	uint16_t x = state;
	state = state * 12829U;

	return ((x >> 7U) ^ x) >> ((x >> 14U) + 3U);
}
// pcg_mcg_32_xsh_rr_16_random_r
uint16_t pcg16(void) {
	static uint32_t state = 0xdc384d8b;
	static uint32_t const inc = 0xbb7dc6ee;

	uint32_t x = state;
	state = state * 747796405U + (inc|1);

	uint16_t value = ((x >> 10U) ^ x) >> 12U;
	uint32_t rot = x >> 28U;
	return (value >> rot) | (value << ((- rot) & 15));
}
// pcg_mcg_32_xsh_rs_16_random_r
uint16_t pcg16_fast(void) {
	static uint32_t state = 0x8fe2b20f;

	uint32_t x = state;
	state = state * 747796405U + 1U;

	return ((x >> 11U) ^ x) >> ((x >> 30U) + 11U);
}
// pcg_mcg_64_xsh_rr_32_random_r
uint32_t pcg32(void) {
	static uint64_t state = 0x385230e79de99aa9;
	static uint64_t const inc = 0xaffa9d760680f66d;

	uint64_t x = state;
	state = state * 6364136223846793005ULL + (inc|1);

	uint32_t value = ((x >> 18U) ^ x) >> 27U;
	uint32_t rot = x >> 59U;
	return (value >> rot) | (value << ((- rot) & 31));
}
// pcg_mcg_64_xsh_rs_32_random_r
uint32_t pcg32_fast(void) {
	static uint64_t state = 0x2c13ae4bbe60e39d;

	uint64_t x = state;
	state = state * 6364136223846793005ULL;

	return ((x >> 22U) ^ x) >> ((x >> 61U) + 22U);
}


// George Marsaglia's XORshift variants
// https://en.wikipedia.org/wiki/Xorshift
uint32_t xorshift32(void) {
	// Seed this 32 bit manually
	static uint32_t a = 0xe326df69;

	uint32_t x = a;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	return a = x;
}
uint64_t xorshift64(void) {
	// Seed this 64 bit manually
	static uint64_t a = 0xa87bf8a757164547;

	uint64_t x = a;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
	return a = x;
}
uint32_t xorshift128(void) {
	// Seed these 32 bit manually
	static uint32_t a = 0xd2b9a87b;
	static uint32_t b = 0xec2cf2be;
	static uint32_t c = 0xdc317cfe;
	static uint32_t d = 0x350ad9e0;

	uint32_t t = d;
	uint32_t const s = a;
	d = c; c = b; b = s;

	t ^= t << 11;
	t ^= t >> 8;
	return a = t ^ s ^ (s >> 19);
}


// Rotation function macros
#define rot64(x,k) (((x) << (k))|((x) >> (64 - (k))))
#define rot32(x,k) (((x) << (k))|((x) >> (32 - (k))))
#define rot16(x,k) (((x) << (k))|((x) >> (16 - (k))))
#define rot8(x,k) (((x) << (k))|((x) >> (8 - (k))))


// https://prng.di.unimi.it/
// Improved versions of George Marsaglia xorshift
// General-purpose xoshiro 32-bit PRNG variants
//uint32_t rot32(uint32_t const x, int const k) { return (x << k) | (x >> (32 - k)); }
// xoshiro128**
uint32_t xoshiro128ss(void) {
	// Seed these 32 bit manually
	static uint32_t s0 = 0x6fe7bc01;
	static uint32_t s1 = 0x964fdc0a;
	static uint32_t s2 = 0xff50ac5b;
	static uint32_t s3 = 0x252d2a75;

	uint32_t const result = rot32(s1 * 5, 7) * 9;
	uint32_t const t = s1 << 9;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;
	s3 = rot32(s3, 11);

	return result;
}
// xoshiro128++
uint32_t xoshiro128pp(void) {
	// Seed these 32 bit manually
	static uint32_t s0 = 0x0ceda153;
	static uint32_t s1 = 0xb0bc8889;
	static uint32_t s2 = 0x83f6c7bf;
	static uint32_t s3 = 0x83fe4c65;

	uint32_t const result = rot32(s0 + s3, 7) + s0;
	uint32_t const t = s1 << 9;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;
	s3 = rot32(s3, 11);

	return result;
}
// xoshiro128+ faster variant
uint32_t xoshiro128p(void) {
	// Seed these 32 bit manually
	static uint32_t s0 = 0x576d09dd;
	static uint32_t s1 = 0x55f240a1;
	static uint32_t s2 = 0xe8f90408;
	static uint32_t s3 = 0xa831286c;

	uint32_t const result = s0 + s3;
	uint32_t const t = s1 << 9;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;
	s3 = rot32(s3, 11);

	return result;

}
// General-purpose xoroshiro 32-bit PRNG variants
// xoroshiro64**
uint32_t xoroshiro64ss(void) {
	// Seed these 32 bit manually
	static uint64_t s0 = 0x5b7fc314;
	static uint64_t s1 = 0xf704b0c6;

	uint32_t const t0 = s0;
	uint32_t t1 = s1;
	uint32_t const result = rot32(t0 * 0x03641019, 5) * 5;

	t1 ^= t0;
	s0 = rot32(t0, 26) ^ t1 ^ (t1 << 9); // a, b
	s1 = rot32(t1, 13); // c

	return result;
}
// xoroshiro64* faster variant
uint32_t xoroshiro64s(void) {
	// Seed these 32 bit manually
	static uint64_t s0 = 0x7016abff;
	static uint64_t s1 = 0x05d1f7c9;

	uint32_t const t0 = s0;
	uint32_t t1 = s1;
	uint32_t const result = t0 * 0x589bb6ca;

	t1 ^= t0;
	s0 = rot32(t0, 26) ^ t1 ^ (t1 << 9); // a, b
	s1 = rot32(t1, 13); // c

	return result;
}

// General-purpose xoshiro 64-bit PRNG variants
//uint64_t rot64(uint64_t const x, int const k) { return (x << k) | (x >> (64 - k)); }
// xoshiro256**
uint64_t xoshiro256ss(void) {
	// Seed these 64 bit manually
	static uint64_t s0 = 0x6f79f27c453f5ce2;
	static uint64_t s1 = 0x2d4f4c8f8a6b81a9;
	static uint64_t s2 = 0x9a7773469eb18730;
	static uint64_t s3 = 0x3a282a6cb9086b8f;

	uint64_t const result = rot64(s1 * 5, 7) * 9;
	uint64_t const t = s1 << 17;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;
	s3 = rot64(s3, 45);

	return result;
}
// xoshiro256++
uint64_t xoshiro256pp(void) {
	// Seed these 64 bit manually
	static uint64_t s0 = 0xe7e6f39e3fe6d812;
	static uint64_t s1 = 0xfe353a4f68cba671;
	static uint64_t s2 = 0x036d46d664128f31;
	static uint64_t s3 = 0x03772003383c1451;

	uint64_t const result = rot64(s0 + s3, 23) + s0;
	uint64_t const t = s1 << 17;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;
	s3 = rot64(s3, 45);

	return result;
}
// xoshiro256+ faster variant
uint64_t xoshiro256p(void) {
	// Seed these 64 bit manually
	static uint64_t s0 = 0x14cd16b6e9df9e27;
	static uint64_t s1 = 0x7831374fca4ed8df;
	static uint64_t s2 = 0x685824bdf7c9dfb9;
	static uint64_t s3 = 0x29264c08627194c1;

	uint64_t const result = s0 + s3;
	uint64_t const t = s1 << 17;

	s2 ^= s0;
	s3 ^= s1;
	s1 ^= s2;
	s0 ^= s3;

	s2 ^= t;
	s3 = rot64(s3, 45);

	return result;
}
// General-purpose xoroshiro 64-bit PRNG variants
// xoroshiro128**
uint64_t xoroshiro128ss(void) {
	// Seed these 64 bit manually
	static uint64_t s0 = 0x116c50456520ca17;
	static uint64_t s1 = 0x004a6c88da91336d;

	uint64_t const t0 = s0;
	uint64_t t1 = s1;
	uint64_t const result = rot64(t0 * 5, 7) * 9;

	t1 ^= t0;
	s0 = rot64(t0, 24) ^ t1 ^ (t1 << 16); // a, b
	s1 = rot64(t1, 37); // c

	return result;
}
// xoroshiro128++
uint64_t xoroshiro128pp(void) {
	// Seed these 64 bit manually
	static uint64_t s0 = 0xf5b0ac38761054b4;
	static uint64_t s1 = 0x2413e73067774b0d;

	uint64_t const t0 = s0;
	uint64_t t1 = s1;
	uint64_t const result = rot64(t0 + t1, 17) + t0;

	t1 ^= t0;
	s0 = rot64(t0, 49) ^ t1 ^ (t1 << 21); // a, b
	s1 = rot64(t1, 28); // c

	return result;
}
// xoroshiro128+ faster variant
uint64_t xoroshiro128p(void) {
	// Seed these 64 bit manually
	static uint64_t s0 = 0x305e0bc4831f6240;
	static uint64_t s1 = 0x7cff19974aef796d;

	uint64_t const t0 = s0;
	uint64_t t1 = s1;
	uint64_t const result = t0 + t1;

	t1 ^= t0;
	s0 = rot64(t0, 24) ^ t1 ^ (t1 << 16); // a, b
	s1 = rot64(t1, 37); // c

	return result;
}


// Brad Forschinger's XORshift
// http://b2d-f9r.blogspot.com/2010/08/16-bit-xorshift-rng-now-with-more.html
uint16_t brad16(void) {
	static uint8_t const a = 5, b = 3, c = 13;
	static uint16_t x = 1, y = 1;
	uint16_t t = (x ^ (x << a));
	x = y;
	return y = (y ^ (y >> c)) ^ (t ^ (t >> b));
}


// @tzarc's 8 bit XORshift, produces minor repeated pattern
// https://github.com/tzarc/qmk_build/blob/bebe5e5b21e99bdb8ff41500ade1eac2d8417d8c/users-tzarc/tzarc_common.c#L57-L63
uint8_t tzarc_prng(void) {
	static uint8_t s = 0xAA, a = 0;
	s ^= s << 3;
	s ^= s >> 5;
	s ^= a++ >> 2;
	return s;
}


// Simple 4-register 8 bit XORshift
// https://github.com/edrosten/8bit_rng
uint8_t xshift8(void) {
	static uint8_t x = 0, y = 0, z = 0, a = 1;
	uint8_t t = x ^ (x << 5);
	x = y; y = z; z = a;
	return a = z ^ ( z >> 1) ^ t ^ (t << 3);
}


// Bob Jenkins Small Fast chaotic PRNG
// http://burtleburtle.net/bob/rand/smallprng.html
uint64_t jsf64(void) {
	static uint64_t a = 0xd336dca32b3f3250;
	static uint64_t b = 0x0c4ce13cca51e105, c = 0x0c4ce13cca51e105, d = 0x0c4ce13cca51e105;

	uint64_t e = a - rot64(b, 7);
	a = b ^ rot64(c, 13);
	b = c + rot64(d, 37);
	c = d + e;
	return d = e + a;
}
uint32_t jsf32(void) {
	static uint32_t a = 0x596fece3;
	static uint32_t b = 0x3028b6ad, c = 0x3028b6ad, d = 0x3028b6ad;

	uint32_t e = a - rot32(b, 27);
	a = b ^ rot32(c, 17);
	b = c + d;
	c = d + e;
	return d = e + a;
}
// https://www.pcg-random.org/posts/bob-jenkins-small-prng-passes-practrand.html
uint16_t jsf16(void) {
	static uint16_t a = 0xf5ae;
	static uint16_t b = 0x290c, c = 0x290c, d = 0x290c;

	uint16_t e = a - rot16(b, 13);
	a = b ^ rot16(c, 8);
	b = c + d;
	c = d + e;
	return d = e + a;
}
uint8_t jsf8(void) {
	static uint8_t a = 0x9d;
	static uint8_t b = 0x78, c = 0x78, d = 0x78;

	uint8_t e = a - rot8(b, 1);
	a = b ^ rot8(c, 4);
	b = c + d;
	c = d + e;
	return d = e + a;
}


int main(int argc, char** argv) {
	uint8_t val8;
	uint16_t val16;
	uint32_t val32;
	uint64_t val64;

	while (1) {
		if (strcmp(argv[1], "pcg8") == 0) { val8=pcg8(); fwrite((void*) &val8, sizeof(val8), 1, stdout); }
		else if (strcmp(argv[1], "pcg8_fast") == 0) { val8 = pcg8_fast(); fwrite((void*) &val8, sizeof(val8), 1, stdout); }
		else if (strcmp(argv[1], "pcg16") == 0) { val16 = pcg16(); fwrite((void*) &val16, sizeof(val16), 1, stdout); }
		else if (strcmp(argv[1], "pcg16_fast") == 0) { val16 = pcg16_fast(); fwrite((void*) &val16, sizeof(val16), 1, stdout); }
		else if (strcmp(argv[1], "pcg32") == 0) { val32 = pcg32(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "pcg32_fast") == 0) { val32 = pcg32_fast(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "xorshift32") == 0) { val32 = xorshift32(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "xorshift64") == 0) { val64 = xorshift64(); fwrite((void*) &val64, sizeof(val64), 1, stdout); }
		else if (strcmp(argv[1], "xorshift128") == 0) { val32 = xorshift128(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "xoshiro128ss") == 0) { val32 = xoshiro128ss(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "xoshiro128pp") == 0) { val32 = xoshiro128pp(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "xoshiro128p") == 0) { val32 = xoshiro128p(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "xoroshiro64ss") == 0) { val32 = xoroshiro64ss(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "xoroshiro64s") == 0) { val32 = xoroshiro64s(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "xoshiro256ss") == 0) { val64 = xoshiro256ss(); fwrite((void*) &val64, sizeof(val64), 1, stdout); }
		else if (strcmp(argv[1], "xoshiro256pp") == 0) { val64 = xoshiro256pp(); fwrite((void*) &val64, sizeof(val64), 1, stdout); }
		else if (strcmp(argv[1], "xoshiro256p") == 0) { val64 = xoshiro256p(); fwrite((void*) &val64, sizeof(val64), 1, stdout); }
		else if (strcmp(argv[1], "xoroshiro128ss") == 0) { val64 = xoroshiro128ss(); fwrite((void*) &val64, sizeof(val64), 1, stdout); }
		else if (strcmp(argv[1], "xoroshiro128pp") == 0) { val64 = xoroshiro128pp(); fwrite((void*) &val64, sizeof(val64), 1, stdout); }
		else if (strcmp(argv[1], "xoroshiro128p") == 0) { val64 = xoroshiro128p(); fwrite((void*) &val64, sizeof(val64), 1, stdout); }
		else if (strcmp(argv[1], "brad16") == 0) { val16 = brad16(); fwrite((void*) &val16, sizeof(val16), 1, stdout); }
		else if (strcmp(argv[1], "tzarc_prng") == 0) { val8=tzarc_prng(); fwrite((void*) &val8, sizeof(val8), 1, stdout); }
		else if (strcmp(argv[1], "xshift8") == 0) { val8=xshift8(); fwrite((void*) &val8, sizeof(val8), 1, stdout); }
		else if (strcmp(argv[1], "jsf8") == 0) { val8=jsf8(); fwrite((void*) &val8, sizeof(val8), 1, stdout); }
		else if (strcmp(argv[1], "jsf16") == 0) { val16 = jsf16(); fwrite((void*) &val16, sizeof(val16), 1, stdout); }
		else if (strcmp(argv[1], "jsf32") == 0) { val32 = jsf32(); fwrite((void*) &val32, sizeof(val32), 1, stdout); }
		else if (strcmp(argv[1], "jsf64") == 0) { val64 = jsf64(); fwrite((void*) &val64, sizeof(val64), 1, stdout); }
		else { return 1; }
	}
	return 0;
}
