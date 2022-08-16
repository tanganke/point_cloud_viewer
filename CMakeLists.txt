cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
  point_cloud_viewer
  VERSION 0.1.0
  DESCRIPTION "Simple OpenGL program to visualize point cloud."
  HOMEPAGE_URL "https://github.com/tanganke/point_cloud_viewer"
  LANGUAGES CXX
)

set(BUILD_TESTING OFF)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----
find_package(glad CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

# ---- Declare executable ----
add_executable(point_cloud_viewer_exe
  source/main.cpp
  source/PointCloud.cpp
  source/Shader.cpp
  source/Window.cpp)
add_executable(point_cloud_viewer::exe ALIAS point_cloud_viewer_exe)

set_target_properties(
  point_cloud_viewer_exe PROPERTIES
  OUTPUT_NAME point_cloud_viewer
  EXPORT_NAME exe
)

target_compile_features(point_cloud_viewer_exe PRIVATE cxx_std_17)

target_link_libraries(point_cloud_viewer_exe PRIVATE glad::glad)
target_link_libraries(point_cloud_viewer_exe PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(point_cloud_viewer_exe PRIVATE glm::glm)
target_link_libraries(point_cloud_viewer_exe PRIVATE imgui::imgui)

# ---- Install rules ----
if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----
if(NOT point_cloud_viewer_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
    AUTHOR_WARNING
    "Developer mode is intended for developers of point_cloud_viewer"
  )
endif()

include(cmake/dev-mode.cmake)
