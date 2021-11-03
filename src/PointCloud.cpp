#include "PointCloud.h"
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>

PointCloud &PointCloud::add_point(const glm::vec3 &p)
{
    auto &lower = bbox[0];
    auto &upper = bbox[1];
    if (p.x < lower.x)
        lower.x = p.x;
    if (p.y < lower.y)
        lower.y = p.y;
    if (p.z < lower.z)
        lower.z = p.z;
    if (p.x > upper.x)
        upper.x = p.x;
    if (p.y > upper.y)
        upper.y = p.y;
    if (p.z > upper.z)
        upper.z = p.z;
    points.push_back(p);
    return *this;
}

PointCloud &PointCloud::load_points(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        spdlog::critical("Could not open cloud point from file {}", filename);
        goto FAIL;
    }

    points.clear();
    glm::vec3 p;
    while (!file.eof())
    {
        if (file >> p.x >> p.y >> p.z)
            add_point(p);
    }

SUCCESS:
    return *this;
FAIL:
    throw std::runtime_error("failed to load point.");
    return *this;
}

PointCloud &PointCloud::load_colors(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        spdlog::critical("Could not open cloud color from file {}", filename);
        goto FAIL;
    }

    colors.clear();
    glm::vec3 c;
    while (!file.eof())
    {
        if (file >> c.x >> c.y >> c.z)
            colors.push_back(c);
    }

SUCCESS:
    return *this;
FAIL:
    throw std::runtime_error("failed to load point colors.");
    return *this;
}

PointCloud &PointCloud::load_normals(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        spdlog::critical("Could not open cloud normal from file {}", filename);
        goto FAIL;
    }

    normals.clear();
    glm::vec3 n;
    while (!file.eof())
    {
        if (file >> n.x >> n.y >> n.z)
            normals.push_back(n);
    }

SUCCESS:
    return *this;
FAIL:
    throw std::runtime_error("failed to load point colors.");
    return *this;
}