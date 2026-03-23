#include "prng.h"

uint64_t PrngSplitMix64(uint64_t x)
{
#define PRNG_SPLIT_MIX_64_X 0x9E3779B97F4A7C15
#define PRNG_SPLIT_MIX_64_A 0xBF58476D1CE4E5B9
#define PRNG_SPLIT_MIX_64_B 0x94D049BB133111EB

    x += PRNG_SPLIT_MIX_64_X;
    x = (x ^ (x >> 30)) * PRNG_SPLIT_MIX_64_A;
    x = (x ^ (x >> 27)) * PRNG_SPLIT_MIX_64_B;

    return x ^ (x >> 31);
}

float SeedToFloat01(uint64_t seed)
{
    seed &= 0x007FFFFF;
    seed |= 0x3F800000;

    uint32_t buffer = (uint32_t)seed;
    float* ptrValue = (float*)(&buffer);

    return *ptrValue - 1.0f;
}