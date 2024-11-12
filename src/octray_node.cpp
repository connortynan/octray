#include "octray_node.hpp"

#include <vector>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

int OctrayNode::intersects(const Vec3f &ray_start, const Vec3f &ray_end) const
{
    Vec3f half_size = {size * 0.5f, size * 0.5f, size * 0.5f};
    Vec3f min = center - half_size;
    Vec3f max = center + half_size;

    if (ray_end.inside(min, max))
        return END_POINT_INSIDE;

    Vec3f dir = ray_end - ray_start;
    float tmin = 0.0f, tmax = 1.0f;

    for (int i = 0; i < 3; ++i)
    {
        // If the ray is parallel to the slab (axis-aligned plane)
        if (std::abs(dir[i]) < 1e-6f)
        {
            // If ray is outside the slab, it doesn't intersect
            if (ray_start[i] < min[i] || ray_start[i] > max[i])
                return NO_INTERSECTION;
        }
        else
        {
            float invD = 1.0f / dir[i];
            float t0 = (min[i] - ray_start[i]) * invD;
            float t1 = (max[i] - ray_start[i]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            tmin = std::max(tmin, t0);
            tmax = std::min(tmax, t1);
            if (tmin > tmax)
                return NO_INTERSECTION;
        }
    }
    return PASSES_THROUGH;
}

void Octray::accumulate_ray(const Vec3f &ray_start, const Vec3f &ray_end, std::vector<CubeInstance> &filledInstances, std::vector<CubeInstance> &outlinedInstances)
{
    std::vector<OctrayNode *> stack;
    stack.push_back(this);

    while (!stack.empty())
    {
        OctrayNode *current = stack.back();
        stack.pop_back();

        int intersection = current->intersects(ray_start, ray_end);
        if (!intersection)
        {
            glm::mat4 transform = glm::mat4{1.0f};
            transform = glm::translate(transform, glm::vec3{current->center.x, current->center.y, current->center.z});
            transform = glm::scale(transform, glm::vec3{std::pow(0.5f, current->depth)});

            outlinedInstances.push_back({transform, {1.f, 1.f, 1.f}});
            continue;
        }

        if (current->leaf)
        {
            if (current->depth >= max_depth)
            {
                glm::mat4 transform = glm::mat4{1.0f};
                transform = glm::translate(transform, glm::vec3{current->center.x, current->center.y, current->center.z});
                transform = glm::scale(transform, glm::vec3{std::pow(0.5f, current->depth)});
                if (intersection == PASSES_THROUGH)
                {
                    filledInstances.push_back({transform, {0.f, 1.f, 0.f}});
                }
                else if (intersection == END_POINT_INSIDE)
                {
                    filledInstances.push_back({transform, {1.f, 0.f, 0.f}});
                }
                continue;
            }
            current->split();
        }
        for (OctrayNode *child : current->children)
        {
            if (child)
            {
                stack.push_back(child);
            }
        }
    }
}