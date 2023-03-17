INCLUDE_DIRECTORIES(${CMAKE_HOME_DIRECTORY}/source/runtime)

set(TARGET model_viewer)

set(TARGET_SRC
    model_viewer.h
    model_viewer.cpp
    main.cpp)

add_executable(${TARGET} WIN32 ${TARGET_SRC})
target_link_libraries(${TARGET} lib_engine)
