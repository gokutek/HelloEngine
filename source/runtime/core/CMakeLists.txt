# ==============================================================================
# 公共头文件
# ==============================================================================
set(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/third_party/)
include_directories(${THIRD_PARTY_ROOT_PATH}/DirectX-Headers/include/directx)
include_directories(${CMAKE_HOME_DIRECTORY}/source/runtime)

# ==============================================================================
# 运行时库
# ==============================================================================
add_library(lib_core
    pch.h
    global.h
    global.cpp
    game_core.h
    game_core.cpp
    graphics_core.h
    graphics_core.cpp
    display.h
    display.cpp
    color.h
    color.cpp
    utility.h
    util/command_line_arg.h
    util/command_line_arg.cpp
    command_list_manager.h
    command_list_manager.cpp
    command_queue.h
    command_queue.cpp
    command_allocator_pool.h
    command_allocator_pool.cpp
    descriptor_heap.h
    descriptor_heap.inl
    descriptor_heap.cpp
    descriptor_allocator.h
    descriptor_allocator.cpp
    dynamic_descriptor_heap.h
    dynamic_descriptor_heap.cpp
    gpu_resource.h
    pixel_buffer.h
    pixel_buffer.cpp
    color_buffer.h
    color_buffer.cpp
    depth_buffer.h
    depth_buffer.cpp
    gpu_buffer.h
    gpu_buffer.cpp
    buffer_manager.h
    buffer_manager.cpp
    command_context.h
    command_context.cpp
    graphics_context.h
    graphics_context.cpp
    compute_context.h
    compute_context.cpp
    command_context_manager.h
    command_context_manager.cpp
    root_signature.h
    root_signature.cpp
    pipeline_state.h
    pipeline_state.cpp
    )

target_precompile_headers(lib_core PRIVATE pch.h)
