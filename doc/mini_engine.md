# MiniEngine
## 目录

# 渲染流程
ModelViewer::RenderScene的主要流程
创建一个GraphicsContext，gfxContext

# DefaultPSO
`Common.hlsli`中定义用于`DefaultVS.hlsl`和`DefaultPS.hlsl`的签名：
```hlsl
#define Renderer_RootSig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_PIXEL), " \
    "DescriptorTable(SRV(t0, numDescriptors = 10), visibility = SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(Sampler(s0, numDescriptors = 10), visibility = SHADER_VISIBILITY_PIXEL)," \
    "DescriptorTable(SRV(t10, numDescriptors = 10), visibility = SHADER_VISIBILITY_PIXEL)," \
    "CBV(b1), " \
    "SRV(t20, visibility = SHADER_VISIBILITY_VERTEX), " \
    "StaticSampler(s10, maxAnisotropy = 8, visibility = SHADER_VISIBILITY_PIXEL)," \
    "StaticSampler(s11, visibility = SHADER_VISIBILITY_PIXEL," \
        "addressU = TEXTURE_ADDRESS_CLAMP," \
        "addressV = TEXTURE_ADDRESS_CLAMP," \
        "addressW = TEXTURE_ADDRESS_CLAMP," \
        "comparisonFunc = COMPARISON_GREATER_EQUAL," \
        "filter = FILTER_MIN_MAG_LINEAR_MIP_POINT)," \
    "StaticSampler(s12, maxAnisotropy = 8, visibility = SHADER_VISIBILITY_PIXEL)"
```
这个声明中用到的寄存器：
* `b0`：CBV，VS可见
* `b0`：CBV，PS可见
* `t0~t9`：SRV，PS可见
* `s0~s9`：Sampler，PS可见
* `t10~t19`：SRV，PS可见
* `b1`：CBV，全部可见
* `t20`：SRV，VS可见
* `s10`：StaticSampler，PS可见
* `s11`：StaticSampler，PS可见
* `s12`：StaticSampler，PS可见

HLSL中声明RootSignature可参考[specifying-root-signatures-in-hlsl](https://learn.microsoft.com/en-us/windows/win32/direct3d12/specifying-root-signatures-in-hlsl)。

`m_RootSig`的初始化：
```c++
m_RootSig.Reset(7, 3);
m_RootSig[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX); //b0，VS可见
m_RootSig[1].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_PIXEL); //b0，PS可见
m_RootSig[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0, 10, D3D12_SHADER_VISIBILITY_PIXEL); //t0~t9
m_RootSig[3].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 0, 10, D3D12_SHADER_VISIBILITY_PIXEL); //s0~s9
m_RootSig[4].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 10, 10, D3D12_SHADER_VISIBILITY_PIXEL); //t10~t19
m_RootSig[5].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_ALL); //b1
m_RootSig[6].InitAsBufferSRV(20, D3D12_SHADER_VISIBILITY_VERTEX); //t20
m_RootSig.InitStaticSampler(10, DefaultSamplerDesc, D3D12_SHADER_VISIBILITY_PIXEL); //s10
m_RootSig.InitStaticSampler(11, SamplerShadowDesc, D3D12_SHADER_VISIBILITY_PIXEL); //s11
m_RootSig.InitStaticSampler(12, CubeMapSamplerDesc, D3D12_SHADER_VISIBILITY_PIXEL); //s12
m_RootSig.Finalize(L"RootSig", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
```

可见，C++中的Root签名与HLSL的声明完全一致。

但是HLSL实际用到的寄存器是它的一个子集？例如下面的例子中，使用到的寄存器是：
* `b0`：cbuffer，VS
* `b1`：cbuffer，VS
* `t20`：VS
* `t0~t4`：PS
* `s0~s4`：PS
* `t10~t13`：PS
* `b0`：cbuffer，PS
* `b1`：cbuffer，PS

`DefaultVS.hlsl`中使用的寄存器：
```c
cbuffer MeshConstants : register(b0)
{
    float4x4 WorldMatrix;   // Object to world
    float3x3 WorldIT;       // Object normal to world normal
};

cbuffer GlobalConstants : register(b1)
{
    float4x4 ViewProjMatrix;
    float4x4 SunShadowMatrix;
    float3 ViewerPos;
    float3 SunDirection;
    float3 SunIntensity;
}

struct Joint
{
    float4x4 PosMatrix;
    float4x3 NrmMatrix; // Inverse-transpose of PosMatrix
};

StructuredBuffer<Joint> Joints : register(t20);
```

`DefaultPS.hlsl`中使用的寄存器：
```c
Texture2D<float4> baseColorTexture          : register(t0);
Texture2D<float3> metallicRoughnessTexture  : register(t1);
Texture2D<float1> occlusionTexture          : register(t2);
Texture2D<float3> emissiveTexture           : register(t3);
Texture2D<float3> normalTexture             : register(t4);

SamplerState baseColorSampler               : register(s0);
SamplerState metallicRoughnessSampler       : register(s1);
SamplerState occlusionSampler               : register(s2);
SamplerState emissiveSampler                : register(s3);
SamplerState normalSampler                  : register(s4);

TextureCube<float3> radianceIBLTexture      : register(t10);
TextureCube<float3> irradianceIBLTexture    : register(t11);
Texture2D<float> texSSAO			        : register(t12);
Texture2D<float> texSunShadow			    : register(t13);

cbuffer MaterialConstants : register(b0)
{
    float4 baseColorFactor;
    float3 emissiveFactor;
    float normalTextureScale;
    float2 metallicRoughnessFactor;
    uint flags;
}

//VS中也有类似的结构体定义
cbuffer GlobalConstants : register(b1)
{
    float4x4 ViewProj;
    float4x4 SunShadowMatrix;
    float3 ViewerPos;
    float3 SunDirection;
    float3 SunIntensity;
    float _pad;
    float IBLRange;
    float IBLBias;
}
```

# FAQ
`_BitScanForward`由低到高搜索哪一位是`1`，返回索引值。`_BitScanReverse`与之相反。
另外还有64位的版本`_BitScanForward64`。

`CommandContext`有`GraphicsContext`和`ComputeContext`两个子类，基类的析构函数也不
是`virtual`的。但子类中都不含任何成员变量。所以也是安全的。

graphic command list
dynamic heap alloc
Resource是如何与hlsl中的寄存器建立关联的？
