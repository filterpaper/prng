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

 /* Simple implementation of Melissa O'Neill's 32-bit PCG PRNG
    https://www.pcg-random.org
  */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng) {
	uint64_t oldstate = rng->state;
	// Advance internal state
	rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
	// Calculate output function (XSH RR), uses old state for max ILP
	uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
	uint32_t rot = oldstate >> 59u;
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void pcg32_srandom_r(pcg32_random_t* rng, uint64_t initstate, uint64_t initseq) {
	//rng->state = 0U;
	rng->state = random()*random();
	rng->inc = (initseq << 1u) | 1u;
	pcg32_random_r(rng);
	rng->state += initstate;
	pcg32_random_r(rng);
}

int main() {
	size_t i;
	pcg32_random_t pcg;

	// Seed using OSX random function
	srandom(time(NULL));
	pcg32_srandom_r(&pcg, random()*random(), random()*random());

	printf("state: 0x%016llx  inc: 0x%016llx\n", pcg.state, pcg.inc);

	for (i = 0; i < 10; i++) {
		printf("0x%08x 0x%08x 0x%08x 0x%08x\n", pcg32_random_r(&pcg), pcg32_random_r(&pcg), pcg32_random_r(&pcg), pcg32_random_r(&pcg));
	}
	//printf("%ld\n", time(NULL));

	return 0;
}
