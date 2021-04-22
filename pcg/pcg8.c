

struct pcg_state_16 {
    uint16_t state;
};

inline void pcg_unique_16_step_r(struct pcg_state_16* rng)
{
	rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_16 + (uint16_t)(((intptr_t)rng) | 1u);
}

inline uint8_t pcg_output_xsh_rs_16_8(uint16_t state)
{
	return (uint8_t)(((state >> 7u) ^ state) >> ((state >> 14u) + 3u));
}

inline uint8_t pcg_oneseq_16_xsh_rs_8_random_r(struct pcg_state_16* rng)
{
    uint16_t oldstate = rng->state;
    //pcg_oneseq_16_step_r(rng);
	rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_16 + (uint16_t)(((intptr_t)rng) | 1u);
    //return pcg_output_xsh_rs_16_8(oldstate);
	return (uint8_t)(((oldstate >> 7u) ^ oldstate) >> ((oldstate >> 14u) + 3u));
}


inline void pcg_mcg_16_step_r(struct pcg_state_16* rng)
{
    rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_16;
}

inline uint8_t pcg_output_xsh_rs_16_8(uint16_t state)
{
    return (uint8_t)(((state >> 7u) ^ state) >> ((state >> 14u) + 3u));
}

inline uint8_t pcg_mcg_16_xsh_rs_8_random_r(struct pcg_state_16* rng)
{
    uint16_t oldstate = rng->state;
    //pcg_mcg_16_step_r(rng);
	rng->state = rng->state * PCG_DEFAULT_MULTIPLIER_16;
    //return pcg_output_xsh_rs_16_8(oldstate);
	return (uint8_t)(((oldstate >> 7u) ^ oldstate) >> ((oldstate >> 14u) + 3u));
}



inline uint8_t pcg_output_rxs_m_16_8(uint16_t state)
{
    return (((state >> ((state >> 13u) + 3u)) ^ state) * 62169u) >> 8u;
}

inline uint8_t pcg_mcg_16_rxs_m_8_random_r(struct pcg_state_16* rng)
{
    uint16_t oldstate = rng->state;
    pcg_mcg_16_step_r(rng);
    return pcg_output_rxs_m_16_8(oldstate);
}


inline uint8_t pcg_rotr_8(uint8_t value, unsigned int rot)
{
/* Unfortunately, clang is kinda pathetic when it comes to properly
 * recognizing idiomatic rotate code, so for clang we actually provide
 * assembler directives (enabled with PCG_USE_INLINE_ASM).  Boo, hiss.
 */
#if PCG_USE_INLINE_ASM && __clang__ && (__x86_64__  || __i386__)
    asm ("rorb   %%cl, %0" : "=r" (value) : "0" (value), "c" (rot));
    return value;
#else
    return (value >> rot) | (value << ((- rot) & 7));
#endif
}

inline uint8_t pcg_output_xsh_rr_16_8(uint16_t state)
{
    return pcg_rotr_8(((state >> 5u) ^ state) >> 5u, state >> 13u);
}

inline uint8_t pcg_mcg_16_xsh_rr_8_random_r(struct pcg_state_16* rng)
{
    uint16_t oldstate = rng->state;
    pcg_mcg_16_step_r(rng);
    return pcg_output_xsh_rr_16_8(oldstate);
}
