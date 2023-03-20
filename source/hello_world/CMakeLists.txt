INCLUDE_DIRECTORIES(${CMAKE_HOME_DIRECTORY}/source/runtime)

set(TARGET hello_world)

set(TARGET_SRC
    hello_world.h
    hello_world.cpp
    main.cpp)

add_executable(${TARGET} WIN32 ${TARGET_SRC})
target_link_libraries(${TARGET} lib_engine)
target_link_libraries(${TARGET} dxgi d3d12 dxguid dinput8 xinput9_1_0)
