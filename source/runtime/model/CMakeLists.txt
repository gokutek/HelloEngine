# ==============================================================================
# 公共头文件
# ==============================================================================
SET(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/third_party/)
INCLUDE_DIRECTORIES(${THIRD_PARTY_ROOT_PATH}/DirectX-Headers/include/directx)
INCLUDE_DIRECTORIES(${CMAKE_HOME_DIRECTORY}/source/runtime)

# ==============================================================================
# 运行时库
# ==============================================================================
add_library(lib_model
    renderer.h
    renderer.cpp
    )
