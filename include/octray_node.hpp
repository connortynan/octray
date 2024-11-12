#include "base_octree_node.hpp"

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct CubeInstance
{
    glm::mat4 model;
    glm::vec3 color;
};

class OctrayNode : public BaseOctreeNode<OctrayNode>
{
private:
    enum IntersectionType
    {
        NO_INTERSECTION = 0,
        PASSES_THROUGH = 1,
        END_POINT_INSIDE = 2
    };

public:
    OctrayNode(const Vec3f &_center, const float _size, const size_t _max_depth)
        : BaseOctreeNode(_center, _size, _max_depth) {}

    OctrayNode(const OctrayNode *_parent, const Vec3f &_center)
        : BaseOctreeNode(_parent, _center) {}

    void accumulate_ray(const Vec3f &ray_start, const Vec3f &ray_end, std::vector<CubeInstance> &filledInstances, std::vector<CubeInstance> &outlinedInstances);

private:
    // Return code is IntersectionType
    int intersects(const Vec3f &ray_start, const Vec3f &ray_end) const;
};