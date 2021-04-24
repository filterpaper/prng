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

/* Simple implementation of Bob Jenkin's small fast 64-bit PRNG
   http://burtleburtle.net/bob/rand/smallprng.html
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef uint64_t u8;
typedef struct ranctx { u8 a; u8 b; u8 c; u8 d; } ranctx;

#define rot(x,k) (((x)<<(k))|((x)>>(64-(k))))
u8 ranval(ranctx *x) {
	u8 e = x->a - rot(x->b, 7);
	x->a = x->b ^ rot(x->c, 13);
	x->b = x->c + rot(x->d, 37);
	x->c = x->d + e;
	return x->d = e + x->a;
}

void raninit(ranctx *x, u8 seed) {
	x->a = seed*random();
	x->b = x->c = x->d = seed;

	for (uint16_t i=seed; i>0; --i) { (void)ranval(x); }
}

int main(int argc, char **argv) {
	ranctx rng;

	// Init using OSX random function as seed
	srandom(time(NULL));
	raninit(&rng, random()*random());

	if (argc>1) { // Binary stream output
		u8 val64;
		while (1) {
			val64 = ranval(&rng);
			fwrite((void*) &val64, sizeof(val64), 1, stdout);
		}
	}
	for (uint8_t i=0; i<16; ++i) {
		printf("0x%016llx 0x%016llx 0x%016llx 0x%016llx\n", ranval(&rng), ranval(&rng), ranval(&rng), ranval(&rng));
	}

	return 0;
}
