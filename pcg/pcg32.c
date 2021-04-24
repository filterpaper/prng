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
	TERMS:
	PCG permuted congruential generator
	MCG multiplicative congruential generator
	XSH xorshift
	RR random rotation
	RS random shift
  */

#define PCG_DEFAULT_MULTIPLIER_8   141U
#define PCG_DEFAULT_MULTIPLIER_16  12829U
#define PCG_DEFAULT_MULTIPLIER_32  747796405U
#define PCG_DEFAULT_MULTIPLIER_64  6364136223846793005ULL


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

// pcg_mcg_32_xsh_rr_16_random_r
uint32_t pcg32_random_r(pcg32_random_t *rng) {
	uint64_t x = rng->state;
	// Advance internal state
	rng->state = x * PCG_DEFAULT_MULTIPLIER_64 + (rng->inc|1);

	// Calculate output function (XSH RR), uses old state for max ILP
	uint32_t shifted = ((x >> 18u) ^ x) >> 27u;
	uint32_t rot = x >> 59u;
	return (shifted >> rot) | (shifted << ((-rot) & 31));
}

void pcg32_srandom_r(pcg32_random_t *rng, uint64_t initstate, uint64_t initseq) {
	rng->state = 0u;
	rng->inc = (initseq << 1u) | 1u;
	for (uint8_t i=initseq; i>0; --i) { (void)pcg32_random_r(rng); }
	rng->state += initstate;
	for (uint8_t i=initstate; i>0; --i) { (void)pcg32_random_r(rng); }
}

int main(int argc, char **argv) {
	pcg32_random_t pcg;

	// Init using OSX random function as seed
	srandom(time(NULL));
	pcg32_srandom_r(&pcg, random()*random(), random()*random());

	if (argc>1) { // Binary stream output
		uint32_t val32;
		while (1) {
			val32 = ranval(&rng);
			fwrite((void*) &val32, sizeof(val32), 1, stdout);
		}
	}
	for (uint8_t i=0; i<16; ++i) {
		printf("0x%08x 0x%08x 0x%08x 0x%08x\n", pcg32_random_r(&pcg), pcg32_random_r(&pcg), pcg32_random_r(&pcg), pcg32_random_r(&pcg));
	}

	return 0;
}
