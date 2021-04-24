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

/* Simple implementation of Bob Jenkin's small fast 8j-bit PRNG
   http://burtleburtle.net/bob/rand/smallprng.html
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef uint8_t u1;
typedef struct { u1 a; u1 b; u1 c; u1 d; } ranctx_t;

#define rot(x,k) (((x)<<(k))|((x)>>(8-(k))))
u1 ranval(ranctx_t* x) {
	u1 e = x->a - rot(x->b, 1);
	x->a = x->b ^ rot(x->c, 4);
	x->b = x->c + x->d;
	x->c = x->d + e;
	return x->d = e + x->a;
}

void raninit(ranctx_t *x, u1 seed) {
	x->a = seed*random();
	x->b = x->c = x->d = seed;

	for (u1 i=seed; i>0; --i) { (void)ranval(x); }
}

int main(int argc, char **argv) {
	ranctx_t rng;

	// Init using OSX random function as seed
	srandom(time(NULL));
	raninit(&rng, random());

	if (argc>1) { // Binary stream output
		u1 val8;
		while (1) {
			val8 = ranval(&rng);
			fwrite((void*) &val8, sizeof(val8), 1, stdout);
		}
	} else {
		for (uint8_t i=0; i<16; ++i) {
			printf("0x%02x 0x%02x 0x%02x 0x%02x\n", ranval(&rng), ranval(&rng), ranval(&rng), ranval(&rng));
		}
	}

	return 0;
}
