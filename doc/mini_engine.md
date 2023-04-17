## 移植问题
`CommandContext`有`GraphicsContext`和`ComputeContext`两个子类，子类中都不含任何成员
变量。基类的析构函数也不是`virtual`的。

## DX12术语
* View：一个Buffer可以对应多个不同的View
* CBV：Constant Buffer View
* SRV：Shader Resource View
* UAV：Unordered Access View
* DSV：Depth Stencil View

创建View的接口形式上都差不多：
```C++
void CreateDepthStencilView(
    [in, optional] ID3D12Resource                      *pResource,
    [in, optional] const D3D12_DEPTH_STENCIL_VIEW_DESC *pDesc,
    [in]           D3D12_CPU_DESCRIPTOR_HANDLE         DestDescriptor
);
```
这个函数的返回值是void，View将创建在第3个handle指向的区域里。

## Root signature

参考`DirectX-Graphics-Samples/Samples/Desktop/D3D12MeshShaders/src/MeshletRender/MeshletMS.hlsl`：

```c
#define ROOT_SIG "CBV(b0), \
                  RootConstants(b1, num32bitconstants=2), \
                  SRV(t0), \
                  SRV(t1), \
                  SRV(t2), \
                  SRV(t3)"

struct Constants
{
    float4x4 World;
    float4x4 WorldView;
    float4x4 WorldViewProj;
    uint     DrawMeshlets;
};

struct MeshInfo
{
    uint IndexBytes;
    uint MeshletOffset;
};

struct Vertex
{
    float3 Position;
    float3 Normal;
};

struct Meshlet
{
    uint VertCount;
    uint VertOffset;
    uint PrimCount;
    uint PrimOffset;
};

//这些对应于ROOT_SIG中的声明
ConstantBuffer<Constants> Globals             : register(b0);
ConstantBuffer<MeshInfo>  MeshInfo            : register(b1);
StructuredBuffer<Vertex>  Vertices            : register(t0);
StructuredBuffer<Meshlet> Meshlets            : register(t1);
ByteAddressBuffer         UniqueVertexIndices : register(t2);
StructuredBuffer<uint>    PrimitiveIndices    : register(t3);
```