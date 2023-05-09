#pragma once

#include "core/pch.h"
#include "core/color.h"
#include "core/gpu_buffer.h"
#include "core/byte_address_buffer.h"
#include "core/descriptor_heap.h"
#include "core/math/bounding_box.h"
#include "core/math/bounding_sphere.h"
#include "core/math/matrix4.h"
#include <memory>
#include <vector>

namespace model_h3d
{
    // Unaligned mirror of MaterialConstants
    struct MaterialConstantData
    {
        float baseColorFactor[4]; // default=[1,1,1,1]
        float emissiveFactor[3]; // default=[0,0,0]
        float normalTextureScale; // default=1
        float metallicFactor; // default=1
        float roughnessFactor; // default=1
        uint32_t flags;
    };

    struct ModelData
    {
        bounding_sphere m_BoundingSphere;
        axis_aligned_bounding_box m_BoundingBox;
        std::vector<byte> m_GeometryData;
        std::vector<byte> m_AnimationKeyFrameData;
        //std::vector<AnimationCurve> m_AnimationCurves;
        //std::vector<AnimationSet> m_Animations;
        std::vector<uint16_t> m_JointIndices;
        std::vector<matrix4> m_JointIBMs;
        //std::vector<MaterialTextureData> m_MaterialTextures;
        std::vector<MaterialConstantData> m_MaterialConstants;
        std::vector<Mesh*> m_Meshes;
        //std::vector<GraphNode> m_SceneGraph;
        std::vector<std::string> m_TextureNames;
        std::vector<uint8_t> m_TextureOptions;
    };

    enum
    {
        attrib_mask_0   = (1 << 0),
        attrib_mask_1   = (1 << 1),
        attrib_mask_2   = (1 << 2),
        attrib_mask_3   = (1 << 3),
        attrib_mask_4   = (1 << 4),
        attrib_mask_5   = (1 << 5),
        attrib_mask_6   = (1 << 6),
        attrib_mask_7   = (1 << 7),
        attrib_mask_8   = (1 << 8),
        attrib_mask_9   = (1 << 9),
        attrib_mask_10  = (1 << 10),
        attrib_mask_11  = (1 << 11),
        attrib_mask_12  = (1 << 12),
        attrib_mask_13  = (1 << 13),
        attrib_mask_14  = (1 << 14),
        attrib_mask_15  = (1 << 15),

        // friendly name aliases
        attrib_mask_position    = attrib_mask_0,
        attrib_mask_texcoord0   = attrib_mask_1,
        attrib_mask_normal      = attrib_mask_2,
        attrib_mask_tangent     = attrib_mask_3,
        attrib_mask_bitangent   = attrib_mask_4,
    };

    enum
    {
        attrib_0    = 0,
        attrib_1    = 1,
        attrib_2    = 2,
        attrib_3    = 3,
        attrib_4    = 4,
        attrib_5    = 5,
        attrib_6    = 6,
        attrib_7    = 7,
        attrib_8    = 8,
        attrib_9    = 9,
        attrib_10   = 10,
        attrib_11   = 11,
        attrib_12   = 12,
        attrib_13   = 13,
        attrib_14   = 14,
        attrib_15   = 15,
        maxAttribs  = 16,

        // friendly name aliases
        attrib_position     = attrib_0,
        attrib_texcoord0    = attrib_1,
        attrib_normal       = attrib_2,
        attrib_tangent      = attrib_3,
        attrib_bitangent    = attrib_4,
    };

    enum
    {
        attrib_format_none = 0,
        attrib_format_ubyte,
        attrib_format_byte,
        attrib_format_ushort,
        attrib_format_short,
        attrib_format_float,
    };

    struct Header
    {
        uint32_t meshCount;
        uint32_t materialCount;
        uint32_t vertexDataByteSize;
        uint32_t indexDataByteSize;
        uint32_t vertexDataByteSizeDepth;
        axis_aligned_bounding_box boundingBox;
    };

    struct Attrib
    {
        uint16_t offset; // byte offset from the start of the vertex
        uint16_t normalized; // if true, integer formats are interpreted as [-1, 1] or [0, 1]
        uint16_t components; // 1-4
        uint16_t format;
    };

    struct Mesh
    {
        axis_aligned_bounding_box boundingBox;

        uint32_t materialIndex;

        uint32_t attribsEnabled;
        uint32_t attribsEnabledDepth;
        uint32_t vertexStride;
        uint32_t vertexStrideDepth;
        Attrib attrib[maxAttribs];
        Attrib attribDepth[maxAttribs];

        uint32_t vertexDataByteOffset;
        uint32_t vertexCount;
        uint32_t indexDataByteOffset;
        uint32_t indexCount;

        uint32_t vertexDataByteOffsetDepth;
        uint32_t vertexCountDepth;
    };

    struct Material
    {
        color diffuse;
        color specular;
        color ambient;
        color emissive;
        color transparent; // light passing through a transparent surface is multiplied by this filter color
        float opacity;
        float shininess; // specular exponent
        float specularStrength; // multiplier on top of specular color
        char texDiffusePath[128];
        char texSpecularPath[128];
        char texEmissivePath[128];
        char texNormalPath[128];
        char texLightmapPath[128];
        char texReflectionPath[128];
        char name[128];
    };

    class model
    {
    public:
        model();
        ~model();

        void Clear();

        bool Load(const std::wstring& filename)
        {
            return LoadH3D(filename);
        }

        bool BuildModel(ModelData& model, const std::wstring& basePath = L"") const;

        uint32_t GetMeshCount() const { return m_Header.meshCount; }

        const Mesh& GetMesh(uint32_t meshIdx) const
        {
            ASSERT(meshIdx < m_Header.meshCount);
            return m_pMesh[meshIdx];
        }

        uint32_t GetMaterialCount() const { return m_Header.materialCount; }

        const Material& GetMaterial(uint32_t materialIdx) const
        {
            ASSERT(materialIdx < m_Header.materialCount);
            return m_pMaterial[materialIdx];
        }

        const axis_aligned_bounding_box& GetBoundingBox() const { return m_Header.boundingBox; }

        uint32_t GetVertexStride() const { return m_VertexStride; }

        const D3D12_VERTEX_BUFFER_VIEW& GetVertexBuffer() const { return m_VertexBuffer; }

        const D3D12_INDEX_BUFFER_VIEW& GetIndexBuffer() const { return m_IndexBuffer; }

        descriptor_handle GetSRVs(uint32_t materialIdx, uint32_t subIdx = 0) const
        {
            return m_SRVs + (materialIdx * 6 + subIdx) * m_SRVDescriptorSize;
        }

        void CreateVertexBufferSRV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) const;

        void CreateIndexBufferSRV(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) const;

        bool LoadH3D(const std::wstring& filename);

        void LoadTextures(const std::wstring& basePath);

    private:
        Header m_Header;
        std::unique_ptr<Mesh[]> m_pMesh;
        std::unique_ptr<Material[]> m_pMaterial;

        descriptor_handle m_SRVs;
        uint32_t m_SRVDescriptorSize;

        // These are used at runtime during rendering
        byte_address_buffer m_GeometryBuffer;

        D3D12_VERTEX_BUFFER_VIEW m_VertexBuffer;
        D3D12_INDEX_BUFFER_VIEW m_IndexBuffer;
        uint32_t m_VertexStride;

        // optimized for depth-only rendering
        D3D12_VERTEX_BUFFER_VIEW m_VertexBufferDepth;
        D3D12_INDEX_BUFFER_VIEW m_IndexBufferDepth;
        uint32_t m_VertexStrideDepth;

        // These are allocated when constructing a model to be exported
        std::unique_ptr<unsigned char[]> m_pVertexData;
        std::unique_ptr<unsigned char[]> m_pIndexData;
        std::unique_ptr<unsigned char[]> m_pVertexDataDepth;
        std::unique_ptr<unsigned char[]> m_pIndexDataDepth;
    };
}
