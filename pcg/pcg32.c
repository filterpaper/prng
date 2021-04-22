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
	rng->state = time(NULL);
	rng->inc = (initseq << 1u) | 1u;
	pcg32_random_r(rng);
	rng->state += initstate;
	pcg32_random_r(rng);
}



uint8_t pcg8(void) {
	static uint16_t state = 0x6832;

	uint16_t oldstate = state;
	state = state * 12829U + 1U;

	uint_fast16_t value = ((oldstate >> 5U) ^ oldstate) >> 5U;
	uint_fast32_t rot = oldstate >> 13U;
	return (value >> rot) | (value << ((- rot) & 7));
}


int main() {
	size_t i;
	pcg32_random_t pcg;
	//pcg32_srandom_r(&pcg, 42u, 52u);
	pcg32_srandom_r(&pcg, time(NULL), time(NULL));
	printf("state: 0x%16llx  inc: 0x%16llx\n", pcg.state, pcg.inc);

	for (i = 0; i < 16; i++) {
		printf("0x%08x\n", pcg32_random_r(&pcg));
		//printf("%u\n", pcg8());
	}

	return 0;
}
