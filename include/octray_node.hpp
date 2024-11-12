#pragma once

#include "base_octree_node.hpp"

#include <vector>
#include <glm/glm.hpp>

class OctrayNode : public BaseOctreeNode<OctrayNode>
{
    friend class Octray;
    friend class BaseOctreeNode<OctrayNode>;

public:
    enum IntersectionType
    {
        NO_INTERSECTION = 0,
        PASSES_THROUGH = 1,
        END_POINT_INSIDE = 2
    };

    OctrayNode(const Vec3f &_center, const float _size)
        : BaseOctreeNode(_center, _size) {}

    OctrayNode(const OctrayNode *_parent, const Vec3f &_center)
        : BaseOctreeNode(_parent, _center) {}

    // Return code is IntersectionType
    int intersects(const Vec3f &ray_start, const Vec3f &ray_end) const;
};

struct CubeInstance
{
    glm::mat4 model;
    glm::vec3 color;
};

class Octray : public OctrayNode
{
public:
    Octray(const Vec3f &_center, const float _size, const size_t _max_depth)
        : OctrayNode(_center, _size), max_depth(_max_depth) {}

    void accumulate_ray(const Vec3f &ray_start, const Vec3f &ray_end, std::vector<CubeInstance> &filledInstances, std::vector<CubeInstance> &outlinedInstances);

private:
    size_t max_depth;
};