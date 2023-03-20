# ==============================================================================
# 公共头文件
# ==============================================================================
SET(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/source/third_party/)
INCLUDE_DIRECTORIES(${THIRD_PARTY_ROOT_PATH}/D3D12MemoryAllocator/include)
INCLUDE_DIRECTORIES(${CMAKE_HOME_DIRECTORY}/source/runtime)

# ==============================================================================
# 运行时库
# ==============================================================================
add_library(lib_engine
    core/pch.h
    core/game_core.h
    core/game_core.cpp
    core/graphics_core.h
    core/graphics_core.cpp
    core/display.h
    core/display.cpp
    core/utility.h
    core/util/command_line_arg.h
    core/util/command_line_arg.cpp
    core/command_list_manager.h
    core/command_list_manager.cpp
    core/command_queue.h
    core/command_queue.cpp
    core/command_allocator_pool.h
    core/command_allocator_pool.cpp
    core/descriptor_heap.h
    core/descriptor_heap.cpp
    core/gpu_resource.h
    core/command_context.h
    core/command_context.cpp
    core/graphics_context.h
    core/graphics_context.cpp
    core/compute_context.h
    core/compute_context.cpp
    )

target_precompile_headers(lib_engine PRIVATE core/pch.h)

#target_compile_definitions(test_lua_ext_lib PRIVATE LUA_BUILD_AS_DLL PRIVATE LUA_LIB)
