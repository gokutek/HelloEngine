# ==============================================================================
# 公共头文件
# ==============================================================================
set(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/third_party/)
include_directories(${THIRD_PARTY_ROOT_PATH}/DirectX-Headers/include/directx)
include_directories(${CMAKE_HOME_DIRECTORY}/source/runtime)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# ==============================================================================
# 运行时库
# ==============================================================================

# 遍历目录下的源文件并保存到SRC_FILES变量中
file(GLOB CPP_SRC_FILES "*.h" "*.cpp")
file(GLOB HLSL_SRC_FILES "shaders/*.hlsli" "shaders/*.hlsl")

add_library(lib_model
    ${CPP_SRC_FILES}
    ${HLSL_SRC_FILES}
    )

# 设置shader文件的分组
source_group("Shader Files" FILES ${HLSL_SRC_FILES})

set(MANUAL_SET_HLSL 0)

if(${MANUAL_SET_HLSL} EQUAL 0)
    include(${CMAKE_HOME_DIRECTORY}/source/common.cmake)
    batch_set_hlsl_file_props("shaders/")
else()
    # 已废弃~仅用于测试批量设置是否有问题
    set_hlsl_file_prop(default_vs Vertex)
    set_hlsl_file_prop(default_ps Pixel)
    set_hlsl_file_prop(skybox_vs Vertex)
    set_hlsl_file_prop(skybox_ps Pixel)
endif()
