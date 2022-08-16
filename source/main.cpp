#include "Window.h"
#include "PointCloud.h"
#include "structopt.hpp"
#include <cstdlib>
#include <iostream>
#include <optional>
#include <filesystem>
namespace fs = std::filesystem;
#include <algorithm>

struct Options {
  std::string                point_cloud;
  std::optional<std::string> normals;
  std::optional<std::string> colors;
};
STRUCTOPT(Options, point_cloud, normals, colors);
Options options;

//-------------- global variables --------------------------------

PointCloud point_cloud;

//-------------- functions ----------------------------------------

int main(int argc, char** argv) {
#ifndef NDEBUG
  spdlog::set_level(spdlog::level::level_enum::debug);
#else
  spdlog::set_level(spdlog::level::level_enum::warn);
#endif

  try {
    options = structopt::app("point_cloud_viewer", "0.1").parse<Options>(argc, argv);
  } catch (structopt::exception& e) {
    std::cout << e.what() << "\n";
    std::cout << e.help();
    exit(EXIT_FAILURE);
  }

  //-------------- initialize Point Cloud --------------------------------
  point_cloud.load_points(options.point_cloud);
  spdlog::debug("PointCloud loaded {} points", point_cloud.get_points().size());

  if (options.colors) {
    point_cloud.load_colors(options.colors.value());
    spdlog::debug("PointCloud loaded {} colors", point_cloud.get_colors().size());
  } else {
    if (options.normals) {
      point_cloud.load_normals(options.normals.value());
      spdlog::debug("PointCloud loaded {} normals", point_cloud.normals.size());
      const std::vector<glm::vec3>& normals = point_cloud.normals;
      std::vector<glm::vec3>        colors(normals.size());
      std::transform(normals.begin(), normals.end(), colors.begin(),
                     [](const glm::vec3& n) { return (glm::normalize(n) + 1.0f) / 2.0f; });
      point_cloud.set_colors(colors);
    } else {
      spdlog::debug("PointCloud loaded no colors, set all colors to black");
      std::vector<glm::vec3> colors(point_cloud.get_points().size(), glm::vec3(0.0f, 0.0f, 0.0f));
      point_cloud.set_colors(colors);
    }
  }

  assert(point_cloud.get_points().size() == point_cloud.get_colors().size());

  //-------------- initialize Window --------------------------------
  Window window("point cloud viewer", 1600, 1000);
  try {
    window.Run();
  } catch (const std::exception& e) {
    spdlog::critical("{}", e.what());
    exit(EXIT_FAILURE);
  }

  return EXIT_SUCCESS;
}