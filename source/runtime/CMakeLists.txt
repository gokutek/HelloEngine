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
    core/game_core.h
    core/game_core.cpp
    core/graphics_core.h
    core/graphics_core.cpp
    core/display.h
    core/display.cpp
    core/utility.h
    )


target_precompile_headers(lib_engine PRIVATE core/pch.h)

#target_compile_definitions(test_lua_ext_lib PRIVATE LUA_BUILD_AS_DLL PRIVATE LUA_LIB)
