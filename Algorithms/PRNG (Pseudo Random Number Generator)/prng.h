#pragma once

#include <stdint.h>

uint64_t PrngSplitMix64(uint64_t x);

float SeedToFloat01(uint64_t seed);