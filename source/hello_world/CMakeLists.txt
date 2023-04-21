SET(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/third_party/)
INCLUDE_DIRECTORIES(${THIRD_PARTY_ROOT_PATH}/DirectX-Headers/include/directx)
INCLUDE_DIRECTORIES(${CMAKE_HOME_DIRECTORY}/source/runtime)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

set(TARGET hello_world)

set(TARGET_SRC
    hello_world.h
    hello_world.cpp
    main.cpp
    hello_world_vs.hlsl
    hello_world_ps.hlsl
    )

set_source_files_properties(hello_world_vs.hlsl PROPERTIES VS_SHADER_TYPE Vertex VS_SHADER_MODEL 6.6 VS_SHADER_ENTRYPOINT main VS_SHADER_OUTPUT_HEADER_FILE ./hello_world_vs.h VS_SHADER_VARIABLE_NAME hello_world_vs_cso)
set_source_files_properties(hello_world_ps.hlsl PROPERTIES VS_SHADER_TYPE Pixel VS_SHADER_MODEL 6.6 VS_SHADER_ENTRYPOINT main VS_SHADER_OUTPUT_HEADER_FILE ./hello_world_ps.h VS_SHADER_VARIABLE_NAME hello_world_ps_cso)

add_executable(${TARGET} WIN32 ${TARGET_SRC})
target_link_libraries(${TARGET} lib_engine)
target_link_libraries(${TARGET} dxgi d3d12 dxguid dinput8 xinput9_1_0)
