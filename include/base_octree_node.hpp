#pragma once

#include <array>
#include <vectors.hpp>

template <typename NodeType>
class BaseOctreeNode
{
public:
    BaseOctreeNode(const Vec3f &_center, const float _size)
        : parent(nullptr), center(_center), size(_size), depth(0)
    {
        children.fill(nullptr);
    }

    virtual ~BaseOctreeNode()
    {
        if (!leaf)
        {
            for (auto &child : children)
            {
                delete child;
                child = nullptr;
            }
        }
    }

protected:
    BaseOctreeNode(const NodeType *_parent, const Vec3f &_center)
        : parent(_parent), center(_center), size(_parent->size / 2.f), depth(_parent->depth + 1)
    {
        children.fill(nullptr);
    }

    void split()
    {
        leaf = false;

        float quarter_size = size / 4.0f;
        for (int i = 0; i < 8; i++)
        {
            Vec3f child_center = center;
            child_center.x += (i & 1 ? quarter_size : -quarter_size);
            child_center.y += (i & 2 ? quarter_size : -quarter_size);
            child_center.z += (i & 4 ? quarter_size : -quarter_size);

            children[i] = new NodeType(static_cast<NodeType *>(this), child_center);
        }
    }

protected:
    bool leaf = true;

    const Vec3f center;
    const float size;

    const size_t depth;
    const NodeType *parent;

    std::array<NodeType *, 8> children;
};