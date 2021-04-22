/* draft PCG codes derived from "pcg-c"

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


// pcg_mcg_16_xsh_rr_8_random_r
uint8_t pcg8(void) {
	static uint16_t state = 0x6835;

	uint16_t oldstate = state;
	state = state * 12829U;

	uint8_t value = ((oldstate >> 5U) ^ oldstate) >> 5U;
	uint32_t rot = oldstate >> 13U;
	return (value >> rot) | (value << ((- rot) & 7));
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

	uint16_t value = ((oldstate >> 10U) ^ oldstate) >> 12U;
	uint32_t rot = oldstate >> 28U;
	return (value >> rot) | (value << ((- rot) & 15));
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
