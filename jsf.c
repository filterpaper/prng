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

/* Combined implementation of Bob Jenkin's small fast PRNG
   http://burtleburtle.net/bob/rand/smallprng.html
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef uint8_t u1;
typedef struct { u1 a; u1 b; u1 c; u1 d; } ranctx8_t;
typedef uint16_t u2;
typedef struct { u2 a; u2 b; u2 c; u2 d; } ranctx16_t;
typedef uint32_t u4;
typedef struct { u4 a; u4 b; u4 c; u4 d; } ranctx32_t;
typedef uint64_t u8;
typedef struct { u8 a; u8 b; u8 c; u8 d; } ranctx64_t;

#define rot8(x,k) (((x)<<(k))|((x)>>(8-(k))))
#define rot16(x,k) (((x)<<(k))|((x)>>(16-(k))))
#define rot32(x,k) (((x)<<(k))|((x)>>(32-(k))))
#define rot64(x,k) (((x)<<(k))|((x)>>(64-(k))))

u1 ranval8(ranctx8_t* x) {
	u1 e = x->a - rot8(x->b, 1);
	x->a = x->b ^ rot8(x->c, 4);
	x->b = x->c + x->d;
	x->c = x->d + e;
	return x->d = e + x->a;
}
void raninit8(ranctx8_t *x, u1 seed) {
	x->a = seed*random();
	x->b = x->c = x->d = seed;

	for (u1 i=seed; i>0; --i) { (void)ranval8(x); }
}

u2 ranval16(ranctx16_t *x) {
	u2 e = x->a - rot16(x->b, 13);
	x->a = x->b ^ rot16(x->c, 8);
	x->b = x->c + x->d;
	x->c = x->d + e;
	return x->d = e + x->a;
}
void raninit16(ranctx16_t *x, u2 seed) {
	x->a = seed*random();
	x->b = x->c = x->d = seed;

	for (u2 i=seed; i>0; --i) { (void)ranval16(x); }
}

u4 ranval32(ranctx32_t *x) {
	u4 e = x->a - rot32(x->b, 27);
	x->a = x->b ^ rot32(x->c, 17);
	x->b = x->c + x->d;
	x->c = x->d + e;
	return x->d = e + x->a;
}
void raninit32(ranctx32_t *x, u4 seed) {
	x->a = seed*random();
	x->b = x->c = x->d = seed;

	for (uint16_t i=seed; i>0; --i) { (void)ranval32(x); }
}

u8 ranval64(ranctx64_t *x) {
	u8 e = x->a - rot64(x->b, 7);
	x->a = x->b ^ rot64(x->c, 13);
	x->b = x->c + rot64(x->d, 37);
	x->c = x->d + e;
	return x->d = e + x->a;
}
void raninit64(ranctx64_t *x, u8 seed) {
	x->a = seed*random();
	x->b = x->c = x->d = seed;

	for (uint16_t i=seed; i>0; --i) { (void)ranval64(x); }
}

int main(int argc, char **argv) {
	ranctx8_t rng8;
	ranctx16_t rng16;
	ranctx32_t rng32;
	ranctx64_t rng64;

	// Init using OSX random function as seed
	srandom(time(NULL));
	raninit8(&rng8, random());
	raninit16(&rng16, random());
	raninit32(&rng32, random());
	raninit64(&rng64, random());

	for (uint8_t i=0; i<8; ++i) {
		for (uint8_t j=0; j<2; ++j)
		{ printf("0x%02x 0x%02x 0x%02x 0x%02x ", ranval8(&rng8), ranval8(&rng8), ranval8(&rng8), ranval8(&rng8)); }
		printf("\n");
	}
	for (uint8_t i=0; i<8; ++i) {
		for (uint8_t j=0; j<2; ++j)
		{ printf("0x%04hx 0x%04hx 0x%04hx 0x%04hx ", ranval16(&rng16), ranval16(&rng16), ranval16(&rng16), ranval16(&rng16)); }
		printf("\n");
	}
	for (uint8_t i=0; i<8; ++i) {
		for (uint8_t j=0; j<2; ++j)
		{ printf("0x%08x 0x%08x 0x%08x 0x%08x ", ranval32(&rng32), ranval32(&rng32), ranval32(&rng32), ranval32(&rng32)); }
		printf("\n");
	}
	for (uint8_t i=0; i<16; ++i) {
		printf("0x%016llx 0x%016llx 0x%016llx 0x%016llx\n", ranval64(&rng64), ranval64(&rng64), ranval64(&rng64), ranval64(&rng64));
	}

	return 0;
}
