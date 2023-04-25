set(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/third_party/)
include_directories(${THIRD_PARTY_ROOT_PATH}/DirectX-Headers/include/directx)
include_directories(${CMAKE_HOME_DIRECTORY}/source/runtime)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(TARGET hello_world)

set(TARGET_SRC
    hello_world.h
    hello_world.cpp
    main.cpp
    hello_world_vs.hlsl
    hello_world_ps.hlsl
    )

# 设置shader源文件的属性    
function(set_hlsl_file_prop fname shader_type)
    set_source_files_properties(${fname}.hlsl PROPERTIES 
        VS_SHADER_TYPE ${shader_type} 
        VS_SHADER_MODEL 6.6 
        VS_SHADER_ENTRYPOINT main 
        VS_SHADER_OUTPUT_HEADER_FILE ./cso/${fname}.h 
        VS_SHADER_VARIABLE_NAME ${fname}_cso)
       
    source_group("Shader Files" FILES ${fname}.hlsl)
endfunction()

set_hlsl_file_prop(hello_world_vs Vertex)
set_hlsl_file_prop(hello_world_ps Pixel)

add_executable(${TARGET} WIN32 ${TARGET_SRC})
target_link_libraries(${TARGET} lib_core)
target_link_libraries(${TARGET} dxgi d3d12 dxguid dinput8 xinput9_1_0)
