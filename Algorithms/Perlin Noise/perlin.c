#include "perlin.h"

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "prng.h"

static uint64_t GetSeedByWHS(int w, int h, int stride)
{
	uint64_t uw = PrngSplitMix64(w);
	uint64_t uh = PrngSplitMix64(h);
	uint64_t us = PrngSplitMix64(stride);

	return PrngSplitMix64(uw * uh * us);
}

static float SmoothStep(float t)
{
	// 6t^5 - 15t^4 + 10t^3
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static float Lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

static float* CreatePerlinSegment(float* perlinBuffer, int resolution, int stride, int xOffset, int yOffset)
{
	int rw = xOffset - xOffset % stride;
	int rh = yOffset - yOffset % stride;
	int sw = rw + stride;
	int sh = rh + stride;

	float radian[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float cosGradient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float sinGradient[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float cosDirection[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float sinDirection[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float dotProduct[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	const pi2 = 2.0f * 3.1415926535897932f;

	radian[0] = pi2 * SeedToFloat01(GetSeedByWHS(rw, rh, stride));
	radian[1] = pi2 * SeedToFloat01(GetSeedByWHS(sw, rh, stride));
	radian[2] = pi2 * SeedToFloat01(GetSeedByWHS(rw, sh, stride));
	radian[3] = pi2 * SeedToFloat01(GetSeedByWHS(sw, sh, stride));

	for (int i = 0; i < 4; ++i)
	{
		cosGradient[i] = cosf(radian[i]);
		sinGradient[i] = sinf(radian[i]);
	}

	int index = yOffset * resolution + xOffset;

	for (int dy = 0; dy < stride; ++dy)
	{
		float fdy = (float)dy / stride;
		float idy = fdy - 1.0f;
		float sfdy = SmoothStep(fdy);

		sinDirection[0] = fdy;
		sinDirection[1] = fdy;
		sinDirection[2] = idy;
		sinDirection[3] = idy;

		for (int dx = 0; dx < stride; ++dx)
		{
			float fdx = (float)dx / stride;
			float idx = fdx - 1.0f;
			float sfdx = SmoothStep(fdx);

			cosDirection[0] = fdx;
			cosDirection[1] = idx;
			cosDirection[2] = fdx;
			cosDirection[3] = idx;

			for (int i = 0; i < 4; ++i)
			{
				dotProduct[i] = cosGradient[i] * cosDirection[i] + sinGradient[i] * sinDirection[i];
			}

			float x0 = Lerp(dotProduct[0], dotProduct[1], sfdx);
			float x1 = Lerp(dotProduct[2], dotProduct[3], sfdx);
			float value = Lerp(x0, x1, sfdy);

			perlinBuffer[index++] = value;
		}

		index = index + resolution - stride;
	}

	return perlinBuffer;
}

float* CreatePerlin(int resolution, int stride)
{
	float* perlinBuffer = (float*)malloc(sizeof(float) * resolution * resolution);

	if (perlinBuffer == NULL)
	{
		assert("Perlin buffer allocation failed.");

		return NULL;
	}

	for (int xOffset = 0; xOffset < resolution; xOffset += stride)
	{
		for (int yOffset = 0; yOffset < resolution; yOffset += stride)
		{
			CreatePerlinSegment(perlinBuffer, resolution, stride, xOffset, yOffset);
		}
	}

	return perlinBuffer;
}

void FreePerlin(float* perlinBuffer)
{
	free(perlinBuffer);
}