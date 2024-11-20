#ifndef BVH_H
#define BVH_H

#include "shapes.h"

class BoundingVolume {
public:
    Vec3 minV; // Minimum corner of the AABB
    Vec3 maxV; // Maximum corner of the AABB

    BoundingVolume() = default;
    BoundingVolume(const Vec3& min, const Vec3& max) : minV(min), maxV(max) {}

    // Expands the current AABB to include another AABB
    void expand(const BoundingVolume& other);

    // Checks if the AABB intersects with a ray
    bool intersects(const Ray& ray) const;

    // Computes a combined AABB that encloses two AABBs
    static BoundingVolume combine(const BoundingVolume& a, const BoundingVolume& b);

    // Computes an AABB for a group of shapes
    static BoundingVolume computeBounds(const std::vector<std::shared_ptr<Shape>>& shapes, int start, int end);

    // Returns the largest axis of the bounding box (0: x, 1: y, 2: z)
    int getLargestAxis() const;
};


// Represents a node in the BVH
struct BVHNode {
    BoundingVolume bounds;                  // The bounding volume enclosing all objects in this node
    std::vector<std::shared_ptr<Shape>> objects; // Objects in this node (only for leaf nodes)
    std::shared_ptr<BVHNode> left;         // Left child node
    std::shared_ptr<BVHNode> right;        // Right child node
    bool isLeaf;                           // True if this is a leaf node

    BVHNode() : isLeaf(false) {}
};

// The BVH class handles construction and traversal
class BVH {
public:
    BVH() = default;

    // Builds the BVH from a list of shapes
    void build(const std::vector<std::shared_ptr<Shape>>& shapes);

    // Traverses the BVH to find the closest intersection
    bool traverse(const Ray& ray, Intersection& nearestIntersection) const;

private:
    std::shared_ptr<BVHNode> root; // Root node of the BVH

    // Helper function to recursively build the BVH
    std::shared_ptr<BVHNode> buildRecursive(std::vector<std::shared_ptr<Shape>>& shapes, int start, int end);

    // Helper function to traverse the BVH recursively
    bool traverseRecursive(const std::shared_ptr<BVHNode>& node, const Ray& ray, Intersection& nearestIntersection) const;
};

#endif // BVH_H
