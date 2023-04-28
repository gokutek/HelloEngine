set(THIRD_PARTY_ROOT_PATH ${CMAKE_HOME_DIRECTORY}/third_party/)
include_directories(${THIRD_PARTY_ROOT_PATH}/DirectX-Headers/include/directx)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(TARGET test_dx_minimal)

set(TARGET_SRC
    test_dx_minimal.cpp
    )

add_executable(${TARGET} WIN32 ${TARGET_SRC})
target_link_libraries(${TARGET} dxgi d3d12 dxguid dinput8 xinput9_1_0)
