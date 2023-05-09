#if 0
#include "model_h3d.h"
#include "core/utility.h"
#include "core/graphics_core.h"
#include "core/graphics_context.h"
#include "core/upload_buffer.h"
#include <fstream>
#include <string.h>
#include <float.h>
#include <map>

namespace model_h3d
{

model::model()
    : m_pMesh(nullptr),
    m_pMaterial(nullptr),
    m_pVertexData(nullptr),
    m_pIndexData(nullptr),
    m_pVertexDataDepth(nullptr),
    m_pIndexDataDepth(nullptr)
{
    Clear();
}

model::~model()
{
    Clear();
}

void model::Clear()
{
    m_GeometryBuffer.destroy();

    m_pMesh.release();
    m_Header.meshCount = 0;

    m_pMaterial.release();
    m_Header.materialCount = 0;

    m_pVertexData.release();
    m_pIndexData.release();
    m_pVertexDataDepth.release();
    m_pIndexDataDepth.release();

    m_Header.vertexDataByteSize = 0;
    m_Header.indexDataByteSize = 0;
    m_Header.vertexDataByteSizeDepth = 0;

    m_Header.boundingBox = axis_aligned_bounding_box();
}

bool model::LoadH3D(const std::wstring& filename)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);

    file.read((char*)&m_Header, sizeof(Header));
    if (!file)
        return false;

    m_pMesh = new Mesh[m_Header.meshCount];
    m_pMaterial = new Material[m_Header.materialCount];

    if (m_Header.meshCount > 0)
    {
        file.read((char*)m_pMesh, sizeof(Mesh) * m_Header.meshCount);
        if (!file)
            return false;
    }
    if (m_Header.materialCount > 0)
    {
        file.read((char*)m_pMaterial, sizeof(Material) * m_Header.materialCount);
        if (!file)
            return false;
    }

    m_VertexStride = m_pMesh[0].vertexStride;
    m_VertexStrideDepth = m_pMesh[0].vertexStrideDepth;
#if _DEBUG
    for (uint32_t meshIndex = 1; meshIndex < m_Header.meshCount; ++meshIndex)
    {
        const Mesh& mesh = m_pMesh[meshIndex];
        ASSERT(mesh.vertexStride == m_VertexStride);
        ASSERT(mesh.vertexStrideDepth == m_VertexStrideDepth);
    }
    for (uint32_t meshIndex = 0; meshIndex < m_Header.meshCount; ++meshIndex)
    {
        const Mesh& mesh = m_pMesh[meshIndex];

        ASSERT( mesh.attribsEnabled ==
            (attrib_mask_position | attrib_mask_texcoord0 | attrib_mask_normal | attrib_mask_tangent | attrib_mask_bitangent) );
        ASSERT(mesh.attrib[0].components == 3 && mesh.attrib[0].format == model::attrib_format_float); // position
        ASSERT(mesh.attrib[1].components == 2 && mesh.attrib[1].format == model::attrib_format_float); // texcoord0
        ASSERT(mesh.attrib[2].components == 3 && mesh.attrib[2].format == model::attrib_format_float); // normal
        ASSERT(mesh.attrib[3].components == 3 && mesh.attrib[3].format == model::attrib_format_float); // tangent
        ASSERT(mesh.attrib[4].components == 3 && mesh.attrib[4].format == model::attrib_format_float); // bitangent

        ASSERT( mesh.attribsEnabledDepth ==
            (attrib_mask_position) );
        ASSERT(mesh.attrib[0].components == 3 && mesh.attrib[0].format == model::attrib_format_float); // position
    }
#endif

    UploadBuffer geomBuffer;

    uint32_t totalBinarySize = m_Header.vertexDataByteSize
        + m_Header.indexDataByteSize
        + m_Header.vertexDataByteSizeDepth
        + m_Header.indexDataByteSize;

    geomBuffer.Create(L"Geometry Upload Buffer", totalBinarySize);
    uint8_t* uploadMem = (uint8_t*)geomBuffer.Map();

    m_pVertexData = uploadMem;
    m_pIndexData = m_pVertexData + m_Header.vertexDataByteSize;
    m_pVertexDataDepth = m_pIndexData + m_Header.indexDataByteSize;
    m_pIndexDataDepth = m_pVertexDataDepth + m_Header.vertexDataByteSizeDepth;

    if (m_Header.vertexDataByteSize > 0 && !file.read((char*)m_pVertexData, m_Header.vertexDataByteSize))
        return false;
    if (m_Header.indexDataByteSize > 0 && !file.read((char*)m_pIndexData, m_Header.indexDataByteSize))
        return false;

    if (m_Header.vertexDataByteSizeDepth > 0 && !file.read((char*)m_pVertexDataDepth, m_Header.vertexDataByteSizeDepth))
        return false;
    if (m_Header.indexDataByteSize > 0 && !file.read((char*)m_pIndexDataDepth, m_Header.indexDataByteSize))
        return false;

    m_GeometryBuffer.Create(L"Geometry Buffer", totalBinarySize, 1, geomBuffer);

    m_VertexBuffer = m_GeometryBuffer.vertex_buffer_view(0, m_Header.vertexDataByteSize, m_VertexStride);
    m_IndexBuffer = m_GeometryBuffer.index_buffer_view(m_pIndexData - m_pVertexData, m_Header.indexDataByteSize, false);
    m_VertexBufferDepth = m_GeometryBuffer.vertex_buffer_view(m_pVertexDataDepth - m_pVertexData, m_Header.vertexDataByteSizeDepth, m_VertexStride);
    m_IndexBufferDepth = m_GeometryBuffer.index_buffer_view(m_pIndexDataDepth - m_pVertexData, m_Header.indexDataByteSize, false);

    m_pVertexData.reset(new unsigned char[m_Header.vertexDataByteSize]);
    m_pIndexData.reset(new unsigned char[m_Header.indexDataByteSize]);
    m_pVertexDataDepth.reset(new unsigned char[m_Header.vertexDataByteSizeDepth]);
    m_pIndexDataDepth.reset(new unsigned char[m_Header.indexDataByteSize]);

    std::memcpy(m_pVertexData, uploadMem, m_Header.vertexDataByteSize);
    uploadMem += m_Header.vertexDataByteSize;
    std::memcpy(m_pIndexData, uploadMem, m_Header.indexDataByteSize);
    uploadMem += m_Header.indexDataByteSize;
    std::memcpy(m_pVertexDataDepth, uploadMem, m_Header.vertexDataByteSizeDepth);
    uploadMem += m_Header.vertexDataByteSizeDepth;
    std::memcpy(m_pIndexDataDepth, uploadMem, m_Header.indexDataByteSize);

    geomBuffer.Unmap();


    LoadTextures(Utility::GetBasePath(filename));

    return true;
}

bool model::BuildModel(ModelData& model, const std::wstring& basePath) const
{
    model.m_SceneGraph.resize(1);

    GraphNode& node = model.m_SceneGraph[0];
    node.xform = Matrix4(kIdentity);
    node.rotation = Quaternion(kIdentity);
    node.scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
    node.matrixIdx = 0;
    node.hasSibling = 0;

    model.m_MaterialTextures.resize(m_Header.materialCount);
    model.m_MaterialConstants.resize(m_Header.materialCount);
    model.m_TextureNames.clear();
    model.m_TextureOptions.clear();

    std::map<std::string, uint16_t> fileNameMap;

    for (uint32_t i = 0; i < m_Header.materialCount; ++i)
    {
        const Material& material = GetMaterial(i);
        MaterialTextureData& textureData = model.m_MaterialTextures[i];
        textureData.addressModes = 0x55555;

        const std::string& baseColorPath = material.texDiffusePath;
        const std::string& metallicRoughnessPath = material.texSpecularPath;
        const std::string& emissivePath = material.texEmissivePath;
        const std::string& normalPath = material.texNormalPath;

        const std::wstring baseColorFullPath = basePath + Utility::UTF8ToWideString(baseColorPath);
        const std::wstring metallicRoughnessFullPath = basePath + Utility::UTF8ToWideString(metallicRoughnessPath);
        const std::wstring emissiveFullPath = basePath + Utility::UTF8ToWideString(emissivePath);
        const std::wstring normalFullPath = basePath + Utility::UTF8ToWideString(normalPath);

        textureData.stringIdx[kBaseColor] = 0xFFFF;
        textureData.stringIdx[kMetallicRoughness] = 0xFFFF;
        textureData.stringIdx[kOcclusion] = 0xFFFF;
        textureData.stringIdx[kEmissive] = 0xFFFF;
        textureData.stringIdx[kNormal] = 0xFFFF;

        bool alphaTest = false;
        const std::string lowerCaseName = Utility::ToLower(baseColorPath);
        if (lowerCaseName.find("thorn") != string::npos ||
            lowerCaseName.find("plant") != string::npos ||
            lowerCaseName.find("chain") != string::npos)
        {
            alphaTest = true;
        }

        // Handle base color texture
        auto mapLookup = fileNameMap.find(baseColorPath);
        if (mapLookup != fileNameMap.end())
        {
            textureData.stringIdx[kBaseColor] = mapLookup->second;
        }
        else if (baseColorPath.size() > 0 && FileExists(baseColorFullPath))
        {
            fileNameMap[baseColorPath] = (uint16_t)model.m_TextureNames.size();
            textureData.stringIdx[kBaseColor] = (uint16_t)model.m_TextureNames.size();
            model.m_TextureNames.push_back(baseColorPath);
            model.m_TextureOptions.push_back(TextureOptions(true, alphaTest, true));
        }

        // Handle occlusionMetallicRoughness texture
        mapLookup = fileNameMap.find(metallicRoughnessPath);
        if (mapLookup != fileNameMap.end())
        {
            textureData.stringIdx[kMetallicRoughness] = mapLookup->second;
        }
        else if (metallicRoughnessPath.size() > 0 && FileExists(metallicRoughnessFullPath))
        {
            fileNameMap[metallicRoughnessPath] = (uint16_t)model.m_TextureNames.size();
            textureData.stringIdx[kMetallicRoughness] = (uint16_t)model.m_TextureNames.size();
            textureData.stringIdx[kOcclusion] = (uint16_t)model.m_TextureNames.size();
            model.m_TextureNames.push_back(metallicRoughnessPath);
            model.m_TextureOptions.push_back(TextureOptions(false, false, true));
        }

        // Handle emissive texture
        mapLookup = fileNameMap.find(emissivePath);
        if (mapLookup != fileNameMap.end())
        {
            textureData.stringIdx[kEmissive] = mapLookup->second;
        }
        else if (emissivePath.size() > 0 && FileExists(emissiveFullPath))
        {
            fileNameMap[emissivePath] = (uint16_t)model.m_TextureNames.size();
            textureData.stringIdx[kEmissive] = (uint16_t)model.m_TextureNames.size();
            model.m_TextureNames.push_back(emissivePath);
            model.m_TextureOptions.push_back(TextureOptions(true, false, true));
        }

        // Handle normal texture
        mapLookup = fileNameMap.find(normalPath);
        if (mapLookup != fileNameMap.end())
        {
            textureData.stringIdx[kNormal] = mapLookup->second;
        }
        else if (normalPath.size() > 0 && FileExists(normalFullPath))
        {
            fileNameMap[normalPath] = (uint16_t)model.m_TextureNames.size();
            textureData.stringIdx[kNormal] = (uint16_t)model.m_TextureNames.size();
            model.m_TextureNames.push_back(normalPath);
            model.m_TextureOptions.push_back(TextureOptions(false, false, true));
        }

        MaterialConstantData& constantData = model.m_MaterialConstants[i];
        constantData.baseColorFactor[0] = 1.0f;
        constantData.baseColorFactor[1] = 1.0f;
        constantData.baseColorFactor[2] = 1.0f;
        constantData.baseColorFactor[3] = 1.0f;
        constantData.emissiveFactor[0] = 1.0f;
        constantData.emissiveFactor[1] = 1.0f;
        constantData.emissiveFactor[2] = 1.0f;
        constantData.metallicFactor = 1.0f;
        constantData.roughnessFactor = 1.0f;
        constantData.normalTextureScale = 1.0f;
        constantData.flags = 0x3C000000;
        if (alphaTest)
            constantData.flags |= 0x60; // twoSided + alphaTest
    }

    ASSERT(model.m_TextureOptions.size() == model.m_TextureNames.size());
    for (size_t ti = 0; ti < model.m_TextureNames.size(); ++ti)
    {
        std::std::wstring fullPath = basePath + Utility::UTF8ToWideString(model.m_TextureNames[ti]);
        CompileTextureOnDemand(fullPath, model.m_TextureOptions[ti]);
    }

    model.m_BoundingSphere = BoundingSphere(kZero);
    model.m_BoundingBox = AxisAlignedBox(kZero);

    // We're going to piggy-back off of the work to compile glTF meshes by pretending that's what
    // we have.
    for (uint32_t i = 0; i < m_Header.meshCount; ++i)
    {
        const Mesh& mesh = GetMesh(i);

        glTF::Accessor PosStream;
        PosStream.dataPtr = m_pVertexData + mesh.vertexDataByteOffset;
        PosStream.stride = mesh.vertexStride;
        PosStream.count = mesh.vertexCount;
        PosStream.componentType = glTF::Accessor::kFloat;
        PosStream.type = glTF::Accessor::kVec3;

        glTF::Accessor UVStream;
        UVStream.dataPtr = m_pVertexData + mesh.vertexDataByteOffset + 12;
        UVStream.stride = mesh.vertexStride;
        UVStream.count = mesh.vertexCount;
        UVStream.componentType = glTF::Accessor::kFloat;
        UVStream.type = glTF::Accessor::kVec2;

        glTF::Accessor NormalStream;
        NormalStream.dataPtr = m_pVertexData + mesh.vertexDataByteOffset + 20;
        NormalStream.stride = mesh.vertexStride;
        NormalStream.count = mesh.vertexCount;
        NormalStream.componentType = glTF::Accessor::kFloat;
        NormalStream.type = glTF::Accessor::kVec3;

        glTF::Accessor IndexStream;
        IndexStream.dataPtr = m_pIndexData + mesh.indexDataByteOffset;
        IndexStream.stride = 2;
        IndexStream.count = mesh.indexCount;
        IndexStream.componentType = glTF::Accessor::kUnsignedShort;
        IndexStream.type = glTF::Accessor::kScalar;

        glTF::Material material;
        material.flags = model.m_MaterialConstants[mesh.materialIndex].flags;
        material.index = mesh.materialIndex;

        glTF::Mesh gltfMesh;
        gltfMesh.primitives.resize(1);

        glTF::Primitive& prim = gltfMesh.primitives[0];
        prim.attributes[glTF::Primitive::kPosition] = &PosStream;
        prim.attributes[glTF::Primitive::kTexcoord0] = &UVStream;
        prim.attributes[glTF::Primitive::kNormal] = &NormalStream;
        prim.indices = &IndexStream;
        prim.material = &material;
        prim.attribMask = 0xB;
        prim.mode = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        memcpy(prim.minPos, &mesh.boundingBox.GetMin(), 12);
        memcpy(prim.maxPos, &mesh.boundingBox.GetMax(), 12);
        prim.minIndex = 0;
        prim.maxIndex = 0;

        BoundingSphere sphereOS;
        AxisAlignedBox boxOS;
        Renderer::CompileMesh(model.m_Meshes, model.m_GeometryData, gltfMesh, 0, Matrix4(kIdentity), sphereOS, boxOS);
        model.m_BoundingSphere = model.m_BoundingSphere.Union(sphereOS);
        model.m_BoundingBox.AddBoundingBox(boxOS);
    }

    return true;
}

void model::CreateVertexBufferSRV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements = (UINT)(m_VertexBuffer.SizeInBytes / sizeof(UINT32));
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    g_Device->CreateShaderResourceView(const_cast<ID3D12Resource*>(m_GeometryBuffer.GetResource()), &srvDesc, cpuHandle);
}

void model::CreateIndexBufferSRV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = (UINT)(m_VertexBuffer.SizeInBytes / sizeof(UINT32));
    srvDesc.Buffer.NumElements = (UINT)(m_IndexBuffer.SizeInBytes / sizeof(UINT32));
    srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    g_Device->CreateShaderResourceView(const_cast<ID3D12Resource*>(m_GeometryBuffer.GetResource()), &srvDesc, cpuHandle);
}

void model::LoadTextures(const std::wstring& basePath)
{
    //TODO:
}

}
#endif
