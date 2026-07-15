# FFT

- FFT, Fast Fourier Transform
- 고속푸리에변환
- 쿨리-튜키 분할 방식으로 구현된 이산푸리에변환 알고리즘 포함

## Result Image

![](https://github.com/nlime3141592/nLimeLab/blob/main/Algorithms/FFT%20(Fast%20Fourier%20Transform)/result-FFT.png?raw=true)

## Usage

- [BMP 코드 참조](https://github.com/nlime3141592/nLimeLab/tree/main/Algorithms/BMP%20File%20IO)

```c
#include "bmp.h"
#include "fft.h"

#include <stdlib.h>
#include <math.h>

// 단일 주파수 sin 함수 샘플 생성
void CreateSample(FTComplex* buffer, int samplesPerSecond, float frequency)
{
    float omega = 2.0f * 3.141592f * frequency;

    for (int i = 0; i < samplesPerSecond; ++i)
    {
        buffer[i].re = sinf(omega * (float)i / (float)samplesPerSecond);
        buffer[i].im = 0.0f;
    }
}

// 주파수응답-그레이스케일 변환
uint8_t Clamp(float value, float min, float max)
{
    if (value < min) return 0;
    if (value > max) return 255;

    float v = (value - min) / (max - min);

    return (uint8_t)(255.0f * v);
}

int main()
{
    const char* file = "C:/MyDirectory/TestImage.bmp";

    int xResolution = 256;
    int yResolution = 64;
    int yMultiplier = 3;

    int samplesPerSecond = xResolution;
    float frequency = 4.5f;

    FTComplex* buffer = (FTComplex*)calloc(samplesPerSecond * yMultiplier, sizeof(FTComplex));
    FTComplex* samplesOrigin = buffer;
    FTComplex* samplesFFT = (buffer + samplesPerSecond);
    FTComplex* samplesIFFT = (buffer + samplesPerSecond * 2);

    // 고속푸리에변환 수행
    CreateSample(samplesOrigin, samplesPerSecond, frequency); // 원래 신호
    fft(samplesFFT, samplesOrigin, samplesPerSecond); // 원래 신호의 FFT 신호
    ifft(samplesIFFT, samplesFFT, samplesPerSecond); // FFT 신호의 IFFT 신호

    // 시각화
    Bmp bmp;

    TryCreateBmp(&bmp, file, xResolution, yResolution * yMultiplier);

    for (int x = 0; x < xResolution; ++x)
    {
        float magOrigin = Magnitude(samplesOrigin[x]);
        float magFFT = Magnitude(samplesFFT[x]) / samplesPerSecond;
        float magIFFT = Magnitude(samplesIFFT[x]);

        uint8_t grayOrigin = Clamp(magOrigin, -1.0f, 1.0f);
        uint8_t grayFFT = Clamp(magFFT, -1.0f, 1.0f);
        uint8_t grayIFFT = Clamp(magIFFT, -1.0f, 1.0f);

        for (int y = 0; y < yResolution; ++y)
        {
            // 원래 신호 시각화
            SetGrayByte(&bmp, grayOrigin, x, y);

            // 원래 신호의 FFT 신호 시각화
            SetGrayByte(&bmp, grayFFT, x, y + yResolution);

            // FFT 신호의 IFFT 신호 시각화
            SetGrayByte(&bmp, grayIFFT, x, y + yResolution * 2);
        }
    }

    Commit(&bmp);
    TryCloseBmp(&bmp);

    free(buffer);
}
```
