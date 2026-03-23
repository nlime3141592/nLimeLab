# BMP File IO

- BMP 파일 처리 코드
- C언어 기반
- 프로그래밍 시 2차원 시각화가 필요하다면 간단히 사용해볼 수 있음

## Usage

### Access BMP File

```c
#include "bmp.h"

void CreateBMP()
{
    BmpBuffer bmpBuffer = { 0 };
    const char* filePath = "C:/MyDirectory/TestImage.bmp";
    int xResolution = 1920;
    int yResolution = 1080;

    CreateBmp(&bmpBuffer, filePath, xResolution, yResolution);

    // Processing code here (1) ...

    // Just save. API didn't release internal FILE* struct.
    WriteBmp(&bmpBuffer);

    // Processing code here (2) ...

    // Save with releasing internal FILE* struct.
    FreeBmp(&bmpBuffer);
}

void OpenBMP()
{
    BmpBuffer bmpBuffer = { 0 };
    const char* filePath = "C:/MyDirectory/TestImage.bmp";

    OpenBmp(&bmpBuffer, filePath);

    // Processing code here ...

    FreeBmp(&bmpBuffer);
}

int main()
{
    CreateBMP();
    OpenBMP();    
}
```

### Change Color

```c
void Fill_UV(BmpBuffer* bmpBuffer)
{
    int xResolution = bmpBuffer->bmpInfoHeader.biWidth;
    int yResolution = bmpBuffer->bmpInfoHeader.biHeight;

    // 24-bit color space. (R8G8B8)
    BmpRgb color = { 0 };

    for (int y = 0; y < yResolution; ++y)
    {
        float v = (float)y / yResolution;

        for (int x = 0; x < xResolution; ++x)
        {
            float u = (float)x / xResolution;

            color.rgbRed = u;
            color.rgbGreen = v;

            SetPixel(bmpBuffer, &color, x, y);
        }
    }
}

int main()
{
    BmpBuffer bmpBuffer = { 0 };
    const char* filePath = "C:/MyDirectory/UV_Map.bmp";
    int xResolution = 1920;
    int yResolution = 1080;

    CreateBmp(&bmpBuffer, filePath, xResolution, yResolution);
    Fill_UV(&bmpBuffer);
    CloseBmp(&bmpBuffer);
}
```