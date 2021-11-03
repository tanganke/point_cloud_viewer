#pragma once
#include <glm/glm.hpp>
#include <limits>
#include <tuple>
#include <vector>
#include <string>
#include <optional>

/**
 * @brief class to represent a point cloud.
 *
 */
class PointCloud
{
private:
    glm::vec3 bbox[2]{glm::vec3{std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()},
                      glm::vec3{std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()}};

public:
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;

public:
    /** add single point*/
    PointCloud &add_point(const glm::vec3 &p);
    /** load point cloud from file */
    PointCloud &load_points(std::string filename);
    /** load point color from file */
    PointCloud &load_colors(std::string filename);
    /** load point normals from file */
    PointCloud &load_normals(std::string filename);

    inline const std::vector<glm::vec3> &get_points() const
    {
        return points;
    }
    inline PointCloud &set_colors(const std::vector<glm::vec3> &colors_)
    {
        colors = colors_;
        return *this;
    }
    inline const std::vector<glm::vec3> &get_colors() const { return colors; }
    inline const glm::vec3 &get_bbox_min() const { return bbox[0]; }
    inline const glm::vec3 &get_bbox_max() const { return bbox[1]; }
    inline std::tuple<glm::vec3, glm::vec3> get_AABB() const { return {bbox[0], bbox[1]}; }
};