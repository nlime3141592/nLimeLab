# Perlin Noise

- 2차원 노이즈 이미지 생성기
- 시공간에 따른 랜덤한 분포를 표현할 때 사용
- 주요 예제: 지형 제작, 랜덤 온도 분포

## Usage

### Create Perlin Noise

```c
#include "perlin.h"

#include <assert.h>

int main()
{
    int resolution = 4096;
    int stride = 512;

    assert(resolution % stride == 0, "stride must be an integer-multiple of resolution.");

    float* perlinBuffer = CreatePerlin(resolution, stride);

    // The variable "i" means i == y * resolution + x;
    int i = 0;

    // This block shows how to loop sequential.
    for (int y = 0; y < resolution; ++y)
    {
        for (int x = 0; x < resolution; ++x)
        {
            // -1/sqrt(2) <= perlinValue <= 1/sqrt(2)
            float perlinValue = perlinBuffer[i];

            ++i;
        }
    }

    // Release memory.
    FreePerlin(perlinBuffer);
}
```

### Visualization

#### External Libraries on This Example

- [BMP File IO](https://github.com/nlime3141592/nLimeLab/tree/main/Algorithms/BMP%20File%20IO)
- [PRNG](https://github.com/nlime3141592/nLimeLab/tree/main/Algorithms/PRNG%20(Pseudo%20Random%20Number%20Generator))

```c
#include "bmp.h"
#include "perlin.h"

#include <assert.h>

int main()
{
    BmpBuffer bmpBuffer = { 0 };
    const char* filePath = "C:/MyDirectory/Perlin.bmp";
    int resolution = 4096;
    int stride = 512;

    CreateBmp(&bmpBuffer, filePath, resolution, resolution);

    assert(resolution % stride == 0, "stride must be an integer-multiple of resolution.");

    float* perlinBuffer = CreatePerlin(resolution, stride);

    // -1/sqrt(2) <= perlinValue <= 1/sqrt(2)
    const float perlinMin = -0.71f;
    const float perlinMax = 0.71f;

    int i = 0;

    for (int y = 0; y < resolution; ++y)
    {
        for (int x = 0; x < resolution; ++x)
        {
            float value = (perlinBuffer[i] - perlinMin) / (perlinMax - perlinMin);
            unsigned char grayscale = (unsigned char)(255.0f * value);

            SetPixelGrayscale(&bmpBuffer, grayscale, x, y);

            ++i;
        }
    }

    CloseBmp(&bmpBuffer);
}
```