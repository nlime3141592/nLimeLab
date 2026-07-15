#include "fft.h"

#include <math.h>
#include <stdlib.h>

// 시간복잡도: O(n)
// 일반적인 알고리즘이라면 내적으로 구현되는데, 고속푸리에변환 공부 중에 구현한 이산푸리에변환 알고리즘임.
static FTComplex _dft(FTComplex* data, int32_t n, int32_t k, int32_t offset, int32_t step)
{
    if (n == 1)
    {
        return data[offset];
    }

    int32_t n2 = n / 2;
    int32_t step2 = step * 2;
    FTComplex c1 = _dft(data, n2, k % n2, offset, step2);
    FTComplex c2 = _dft(data, n2, k % n2, offset + step, step2);

    float radian = -2.0f * 3.1415926535897932f * (float)k / (float)n;
    float c = cosf(radian);
    float s = sinf(radian);

    FTComplex out;
    out.re = c1.re + c * c2.re - s * c2.im;
    out.im = c1.im + s * c2.re + c * c2.im;
    return out;
}

FTComplex dftSingle(FTComplex* data, int32_t n, int32_t k)
{
    return _dft(data, n, k, 0, 1);
}

FTComplex idftSingle(FTComplex* data, int32_t n, int32_t k)
{
    FTComplex out = _dft(data, n, -k, 0, 1);

    out.re /= (float)n;
    out.im /= (float)n;

    return out;
}

void dft(FTComplex* out, FTComplex* data, int32_t n)
{
    for (int32_t i = 0; i < n; ++i)
    {
        out[i] = dftSingle(data, n, i);
    }
}

void idft(FTComplex* out, FTComplex* data, int32_t n)
{
    for (int32_t i = 0; i < n; ++i)
    {
        out[i] = idftSingle(data, n, i);
    }
}

static void _fft(
    FTComplex* buffer, FTComplex* twiddle,
    int32_t iBegInclusive, int32_t n, int32_t nMax)
{
    // NOTE: buffer는 bit reverse가 적용된 데이터 배열입니다.
    int32_t n2 = n / 2;
    int32_t step = nMax / n;

    if (n > 2)
    {
        _fft(buffer, twiddle, iBegInclusive, n2, nMax);
        _fft(buffer, twiddle, iBegInclusive + n2, n2, nMax);
    }
    
    // butterfly operation
    for (int32_t i = 0; i < n2; ++i)
    {
        int32_t i0 = iBegInclusive + i;
        int32_t i1 = i0 + n2;

        FTComplex even;
        FTComplex odd;
        FTComplex tw = twiddle[i * step];

        even.re = buffer[i0].re;
        even.im = buffer[i0].im;
        odd.re = buffer[i1].re * tw.re - buffer[i1].im * tw.im;
        odd.im = buffer[i1].re * tw.im + buffer[i1].im * tw.re;

        buffer[i0].re = even.re + odd.re;
        buffer[i0].im = even.im + odd.im;
        buffer[i1].re = even.re - odd.re;
        buffer[i1].im = even.im - odd.im;
    }
}

static int32_t ReverseBit(int32_t i, int32_t bitCount)
{
    int32_t j = 0;

    while (bitCount > 0)
    {
        j <<= 1;
        j |= (i & 1);
        i >>= 1;

        --bitCount;
    }

    return j;
}

static FTComplex* CreateTwiddle(int32_t n)
{
    int32_t n2 = n / 2;
    FTComplex* twiddle = (FTComplex*)malloc(sizeof(FTComplex) * n2);

    twiddle[0].re = 1.0f;
    twiddle[0].im = 0.0f;

    if (n2 > 1)
    {
        float angle = -2.0f * 3.1415926535897932f / (float)n;

        twiddle[1].re = cosf(angle);
        twiddle[1].im = sinf(angle);
    }

    for (int32_t i = 2; i < n2; ++i)
    {
        int32_t j = i - 1;

        twiddle[i].re = twiddle[j].re * twiddle[1].re - twiddle[j].im * twiddle[1].im;
        twiddle[i].im = twiddle[j].re * twiddle[1].im + twiddle[j].im * twiddle[1].re;
    }

    return twiddle;
}

void fft(FTComplex* out, FTComplex* data, int32_t n)
{
    FTComplex* twiddle = CreateTwiddle(n);
    int32_t bitCount = (int32_t)log2f(n);

    for (int i = 0; i < n; ++i)
    {
        out[i] = data[ReverseBit(i, bitCount)];
    }

    _fft(out, twiddle, 0, n, n);

    free(twiddle);
}

void ifft(FTComplex* out, FTComplex* data, int32_t n)
{
    FTComplex* twiddle = CreateTwiddle(n);
    int32_t bitCount = (int32_t)log2f(n);

    for (int i = 0; i < n; ++i)
    {
        out[i] = data[ReverseBit(i, bitCount)];
        out[i].im = -out[i].im;
    }

    _fft(out, twiddle, 0, n, n);

    free(twiddle);

    for (int i = 0; i < n; ++i)
    {
        out[i].re /= (float)n;
        out[i].im = -out[i].im / (float)n;
    }
}

float Magnitude(FTComplex c)
{
    return sqrtf(c.re * c.re + c.im * c.im);
}