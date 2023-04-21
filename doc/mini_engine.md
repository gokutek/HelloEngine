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

## CommandList->Close时报错
命令进入列表时并不会报错，错误需要在关闭命令列表时才会触发。

```
D3D12 ERROR: ID3D12CommandList::ResourceBarrier: Before state (0xCDCDCDCD: D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER|D3D12_RESOURCE_STATE_RENDER_TARGET|D3D12_RESOURCE_STATE_UNORDERED_ACCESS|D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE|D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE|D3D12_RESOURCE_STATE_STREAM_OUT|D3D12_RESOURCE_STATE_COPY_DEST|D3D12_RESOURCE_STATE_COPY_SOURCE|D3D12_RESOURCE_STATE_VIDEO_DECODE_READ|D3D12_RESOURCE_STATE_VIDEO_PROCESS_READ|D3D12_RESOURCE_STATE_VIDEO_PROCESS_WRITE|D3D12_RESOURCE_STATE_VIDEO_ENCODE_WRITE|D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE) of resource (0x000002A4643DDB80:'Primary SwapChain Buffer') (subresource: 0) specified by transition barrier does not match with the state (0x4: D3D12_RESOURCE_STATE_RENDER_TARGET) specified in the previous call to ResourceBarrier [ RESOURCE_MANIPULATION ERROR #527: RESOURCE_BARRIER_BEFORE_AFTER_MISMATCH]
0x00007FFED28AFDEC 处(位于 hello_world.exe 中)引发的异常: Microsoft C++ 异常: _com_error，位于内存位置 0x000000CA18CFEFC0 处。
Assertion failed!
```

```C++
D3D12_RESOURCE_BARRIER& barrier_desc = resource_barrier_buffers_[num_barriers_to_flush_++];
barrier_desc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
barrier_desc.Transition.pResource = resource.get_resource();
barrier_desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
barrier_desc.Transition.StateAfter = old_state; //应该是StateBefore
barrier_desc.Transition.StateAfter = new_state;
```
