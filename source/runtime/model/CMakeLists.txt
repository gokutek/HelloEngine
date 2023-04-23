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

# 设置shader源文件的属性    
function(set_hlsl_file_prop fname shader_type)
    set_source_files_properties(shaders/${fname}.hlsl PROPERTIES VS_SHADER_TYPE ${shader_type} VS_SHADER_MODEL 6.6 VS_SHADER_ENTRYPOINT main VS_SHADER_OUTPUT_HEADER_FILE ./cso/${fname}.h VS_SHADER_VARIABLE_NAME ${fname}_cso)
endfunction()

###############################################################################
# 遍历目录下的hlsl文件，调用上面的函数设置他们的属性。
# 目的是避免新增hlsl需要类似下面的手动调用：
#   set_hlsl_file_prop(default_vs Vertex)
#   set_hlsl_file_prop(default_ps Pixel)
###############################################################################
function(batch_set_hlsl_file_props)
    # 遍历文件
    file(GLOB hlsl_files "shaders/*.hlsl")
    foreach(fpath ${hlsl_files})
        string(FIND ${fpath} "/" last_slash_index REVERSE)      # 查找最右一个'/'
        math(EXPR last_slash_index "${last_slash_index} + 1")   # last_slash_index++
        string(SUBSTRING ${fpath} ${last_slash_index} -1 fname) # 截取'/'后的文件名
        string(REPLACE ".hlsl" "" fname_no_ext ${fname})        # 去除文件扩展名
        string(FIND ${fname_no_ext} "_vs" is_vs)                # 查找`_vs`
        string(FIND ${fname_no_ext} "_ps" is_ps)                # 查找`_ps`
        if(NOT ${is_vs} EQUAL -1)
            set_hlsl_file_prop(${fname_no_ext} Vertex)
            message(STATUS "find vertex shader: ${fname_no_ext}")
        elseif(NOT ${is_ps} EQUAL -1)
            set_hlsl_file_prop(${fname_no_ext} Pixel)
            message(STATUS "find pixel shader: ${fname_no_ext}")
        endif()
    endforeach()
endfunction()

set(MANUAL_SET_HLSL 0)

if(${MANUAL_SET_HLSL} EQUAL 0)
    batch_set_hlsl_file_props()
else()
    # 已废弃~仅用于测试批量设置是否有问题
    set_hlsl_file_prop(default_vs Vertex)
    set_hlsl_file_prop(default_ps Pixel)
    set_hlsl_file_prop(skybox_vs Vertex)
    set_hlsl_file_prop(skybox_ps Pixel)
endif()
