#ifndef __TERRAIN_CHUNK_HLSL__
#define __TERRAIN_CHUNK_HLSL__

#define UV_EPSILON 1e-6

// TileMix.hlsl
// 타일 경계에서 모서리와 꼭짓점을 믹스하는 알고리즘

// 공식 문서
// https://docs.unity3d.com/Packages/com.unity.shadergraph@10.8/manual/Texture-2D-Asset-Node.html

float GetTileIndex(UnityTexture2D imap, UnitySamplerState sampler_imap, float2 uv, float du, float dv)
{
    uv.x = clamp(0.0, 1.0 - UV_EPSILON, uv.x + du + UV_EPSILON);
    uv.y = clamp(0.0, 1.0 - UV_EPSILON, uv.y + dv + UV_EPSILON);
    
    float i = imap.Sample(sampler_imap, float2(uv.x, uv.y)).r;

    return round(i * 65535.0);
}

uint GetNeighborTileIndex(int i, int w, int h, int wh, int dx, int dy)
{
    int base = i / wh;
    int offset = i % wh;
    
    int x = offset % w;
    int y = offset / w;
    
    x = (x + w + dx) % w;
    y = (y + h + dy) % h;
    
    return base * wh + y * w + x;
}

float4 GetMixedColorCorner(float4x4 color, int4 index, float2 uvOrigin, float mix)
{
    int imax = max(max(index[0], index[1]), max(index[2], index[3]));
    
    float m0 = (index[0] < index[1]);
    float m1 = (index[0] < index[2]);
    float m2 = (imax == index[3]);
    float m3 = (1.0 - m2) * (1.0 - max(m0, m1));
    float m4 = (1.0 - m2 - m3) * (index[1] > index[2]);
    float m5 = (1.0 - m2 - m3 - m4);
    
    float u = (uvOrigin.x + mix - 1.0) / mix;
    float v = (uvOrigin.y + mix - 1.0) / mix;
    
    u = smoothstep(0.0, 1.0, u);
    v = smoothstep(0.0, 1.0, v);

    float4 c0 = color[0];
    float4 c1 = color[1] * m0 + color[0] * (1.0 - m0);
    float4 c2 = color[2] * m1 + color[0] * (1.0 - m1);
    float4 c3 = color[3] * m2 + color[0] * m3 + color[1] * m4 + color[2] * m5;

    return lerp(lerp(c0, c1, u), lerp(c2, c3, u), v);
}

float4 GetMixedColorEdge(float4x4 color, int4 index, float2 xy, float mix)
{
    bool m0 = (index[0] < index[1]);
    bool m1 = (xy.x >= 1.0 - mix);
    bool m2 = (xy.y >= mix);
    bool m3 = (xy.y <= 1.0 - mix);
    float mask = m0 * m1 * m2 * m3;
    
    float w = (xy.x + mix - 1.0) / mix;
    w = smoothstep(0.0, 1.0, w);
    
    float4 c0 = color[0];
    float4 c1 = color[1] * mask + color[0] * (1.0 - mask);

    return lerp(c0, c1, w);
}

float4 RenderTerrainChunk(
    UnityTexture2DArray tmap,
    UnitySamplerState sampler_tmap,
    UnityTexture2D imap,
    UnitySamplerState sampler_imap,
    float4 uv,
    float w, float h,
    float mix,
    float chunkSize
)
{
    uint k = (uint) round(imap.Sample(sampler_imap, float2(uv.x, uv.y)).r * 65535.0);
    
    float2 uvGlobal = float2(uv.x * chunkSize, uv.y * chunkSize);
    uvGlobal = min(uvGlobal, chunkSize - UV_EPSILON); // 경계에서의 이미지 잘림 방지

    float2 uvPosition = floor(uvGlobal);
    float2 uvLocal = uvGlobal - uvPosition;
    
    int wh = w * h;
    
    float du = 1.0 / (3.0 * chunkSize);
    float dv = 1.0 / (3.0 * chunkSize);
    float2 uvOrigin = float2(uvPosition.x / chunkSize, uvPosition.y / chunkSize);
    
    uint i0 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, du, dv);
    uint i1 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, 2 * du, dv);
    uint i2 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, 2 * du, 2 * dv);
    uint i3 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, du, 2 * dv);
    uint i4 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, 0, 2 * dv);
    uint i5 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, 0, dv);
    uint i6 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, 0, 0);
    uint i7 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, du, 0);
    uint i8 = (uint) GetTileIndex(imap, sampler_imap, uvOrigin, 2 * du, 0);
    
    float4 c0 = tmap.Sample(sampler_tmap, float3(uvLocal, i0));
    float4 c1 = tmap.Sample(sampler_tmap, float3(uvLocal, i1));
    float4 c2 = tmap.Sample(sampler_tmap, float3(uvLocal, i2));
    float4 c3 = tmap.Sample(sampler_tmap, float3(uvLocal, i3));
    float4 c4 = tmap.Sample(sampler_tmap, float3(uvLocal, i4));
    float4 c5 = tmap.Sample(sampler_tmap, float3(uvLocal, i5));
    float4 c6 = tmap.Sample(sampler_tmap, float3(uvLocal, i6));
    float4 c7 = tmap.Sample(sampler_tmap, float3(uvLocal, i7));
    float4 c8 = tmap.Sample(sampler_tmap, float3(uvLocal, i8));
    
    float b0 = mix;
    float b1 = 1.0 - mix;

    float m0 = uvLocal.x <= b0; // left border
    float m1 = uvLocal.x >= b1; // right border
    float m2 = uvLocal.y <= b0; // bottom border
    float m3 = uvLocal.y >= b1; // top border
    float m4 = !(m0 || m1 || m2 || m3); // internal
    float m5 = m1 * m3; // rt corner
    float m6 = m0 * m3; // lt corner
    float m7 = m0 * m2; // lb corner
    float m8 = m1 * m2; // rb corner
    float mCorner = m5 + m6 + m7 + m8;
    float mEdge = 1.0 - (m4 + mCorner);
    
    float4x4 cSet0 = float4x4(c0, c1, c3, c2);
    float4x4 cSet1 = float4x4(c0, c3, c5, c4);
    float4x4 cSet2 = float4x4(c0, c5, c7, c6);
    float4x4 cSet3 = float4x4(c0, c7, c1, c8);

    int4 iSet0 = int4(i0, i1, i3, i2) / wh;
    int4 iSet1 = int4(i0, i3, i5, i4) / wh;
    int4 iSet2 = int4(i0, i5, i7, i6) / wh;
    int4 iSet3 = int4(i0, i7, i1, i8) / wh;

    float2 uv0 = float2(uvLocal.x, uvLocal.y);
    float2 uv1 = float2(uvLocal.y, 1 - uvLocal.x);
    float2 uv2 = float2(1 - uvLocal.x, 1 - uvLocal.y);
    float2 uv3 = float2(1 - uvLocal.y, uvLocal.x);
    
    float4 fc00 = m1 * GetMixedColorEdge(cSet0, iSet0, uv0, mix);
    float4 fc10 = m3 * GetMixedColorEdge(cSet1, iSet1, uv1, mix);
    float4 fc20 = m0 * GetMixedColorEdge(cSet2, iSet2, uv2, mix);
    float4 fc30 = m2 * GetMixedColorEdge(cSet3, iSet3, uv3, mix);
    
    float4 fc01 = m5 * GetMixedColorCorner(cSet0, iSet0, uv0, mix);
    float4 fc11 = m6 * GetMixedColorCorner(cSet1, iSet1, uv1, mix);
    float4 fc21 = m7 * GetMixedColorCorner(cSet2, iSet2, uv2, mix);
    float4 fc31 = m8 * GetMixedColorCorner(cSet3, iSet3, uv3, mix);
    
    return c0 * m4 + (fc00 + fc10 + fc20 + fc30) * mEdge + (fc01 + fc11 + fc21 + fc31) * mCorner;
}

void RenderTerrainChunk_float(
    UnityTexture2DArray tmap,
    UnitySamplerState sampler_tmap,
    UnityTexture2D imap,
    UnitySamplerState sampler_imap,
    float4 uv,
    float w, float h,
    float mix,
    float chunkSize,
    out float4 color)
{
    color = RenderTerrainChunk(
        tmap, sampler_tmap,
        imap, sampler_imap,
        uv,
        w, h,
        mix,
        chunkSize);
}

#endif