# ==============================================================================
# 公共头文件
# ==============================================================================
SET(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/third_party/)
INCLUDE_DIRECTORIES(${THIRD_PARTY_ROOT_PATH}/DirectX-Headers/include/directx)
INCLUDE_DIRECTORIES(${CMAKE_HOME_DIRECTORY}/source/runtime)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# ==============================================================================
# 运行时库
# ==============================================================================

# 遍历目录下的源文件并保存到SRC_FILES变量中
FILE(GLOB CPP_SRC_FILES "*.h" "*.cpp")
FILE(GLOB HLSL_SRC_FILES "shaders/*.hlsli" "shaders/*.hlsl")

add_library(lib_model
    ${CPP_SRC_FILES}
    ${HLSL_SRC_FILES}
    )

source_group("Shader Files" FILES ${HLSL_SRC_FILES})

# 设置shader源文件的属性    
function(set_hlsl_file_prop fname shader_type)
    set_source_files_properties(shaders/${fname}.hlsl PROPERTIES 
        VS_SHADER_TYPE ${shader_type} 
        VS_SHADER_MODEL 6.6 
        VS_SHADER_ENTRYPOINT main 
        VS_SHADER_OUTPUT_HEADER_FILE ./cso/${fname}.h 
        VS_SHADER_VARIABLE_NAME ${fname}_cso)
endfunction()

set_hlsl_file_prop(default_vs Vertex)
set_hlsl_file_prop(default_ps Pixel)
set_hlsl_file_prop(skybox_vs Vertex)
set_hlsl_file_prop(skybox_ps Pixel)
