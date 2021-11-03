project(point_cloud_viewer)
cmake_minimum_required(VERSION 3.15)
set(TARGET_NAME point_cloud_viewer)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES src/*.cpp)
add_executable(${TARGET_NAME} ${SOURCES})
target_include_directories(${TARGET_NAME} PRIVATE include)
target_link_libraries(${TARGET_NAME} imgui glfw glad spdlog)
# target_compile_definitions(${TARGET_NAME} PUBLIC 
#     ASSERTS_DIR="${CMAKE_SOURCE_DIR}/Asserts/"
#     APPLICATION_NAME="${TARGET_NAME}"
# )

if(UNIX)
    target_link_libraries(${TARGET_NAME} dl)
endif()