#pragma once

#include "core/math/vector.h"
#include <vector>
#include <string>

/**
 * @brief 加载简化版的obj格式模型
 * 
 * https://github.com/gokutek/tinyrenderer/blob/master/model.h
 */
class model_obj 
{
public:
    /**
     * @brief 构造函数，加载模型资源
     * @param filename 文件路径
    */
    model_obj(std::string const& filename);
    
    /**
     * @brief 顶点数
     * @return 
    */
    size_t nverts() const;

    /**
     * @brief 面数
     * @return 
    */
    size_t nfaces() const;

    /**
     * @brief 获取第i个面的第j个顶点的法向量
     * @param face 
     * @param nth_vert 
     * @return 
    */
    vector3 normal(int face, int nth_vert) const;

    /**
     * @brief 获取顶点
     * @param i 
     * @return 
    */
    vector3 vert(int i) const;

    /**
     * @brief 获取第i个面的第j个顶点的坐标
     * @param face 
     * @param nth_vert 
     * @return 
    */
    vector3 vert(int face, int nth_vert) const;

    /**
     * @brief 获取第i个面的第j个顶点的UV
     * @param face 
     * @param nth_vert 
     * @return 
    */
    vector2 uv(int face, int nth_vert) const;

private:
    std::vector<vector3> verts; ///< 顶点
    std::vector<vector3> norms; ///< 法向量
    std::vector<vector2> tex_coord; ///< UV坐标
    std::vector<int> facet_vrt; ///< 顶点索引
    std::vector<int> facet_tex; ///< UV索引
    std::vector<int> facet_nrm; ///< 法向量索引
};
