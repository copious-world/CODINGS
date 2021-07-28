project(CODINGS)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
set_target_properties(${TARGET_NAME} PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} ${SRC_LIST})

