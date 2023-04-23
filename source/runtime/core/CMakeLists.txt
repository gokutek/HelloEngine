# ==============================================================================
# 公共头文件
# ==============================================================================
set(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/third_party/)
include_directories(${THIRD_PARTY_ROOT_PATH}/DirectX-Headers/include/directx)
include_directories(${CMAKE_HOME_DIRECTORY}/source/runtime)

# ==============================================================================
# 运行时库
# ==============================================================================
file(GLOB_RECURSE CPP_SRC_FILES "*.h" "*.cpp")

add_library(lib_core
    ${CPP_SRC_FILES}
    )

target_precompile_headers(lib_core PRIVATE pch.h)
