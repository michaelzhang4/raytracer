#include "bvh.h"

// Expands the current AABB to include another AABB
void BoundingVolume::expand(const BoundingVolume& other) {
    minV = minV.min(other.minV);
    maxV = maxV.max(other.maxV);
}

// Checks if the AABB intersects with a ray
bool BoundingVolume::intersects(const Ray& ray) const {
    float tmin = std::numeric_limits<float>::lowest();
    float tmax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i) {
        float invDir = 1.0f / ray.direction[i];
        float t1 = (minV[i] - ray.origin[i]) * invDir;
        float t2 = (maxV[i] - ray.origin[i]) * invDir;

        if (t1 > t2) std::swap(t1, t2);
        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax) {
            return false;
        }
    }

    return true;
}

// Computes a combined AABB that encloses two AABBs
BoundingVolume BoundingVolume::combine(const BoundingVolume& a, const BoundingVolume& b) {
    Vec3 combinedMin = a.minV.min(b.minV);
    Vec3 combinedMax = b.maxV.max(b.maxV);
    return BoundingVolume(combinedMin, combinedMax);
}

BoundingVolume BoundingVolume::computeBounds(const std::vector<std::shared_ptr<Shape>>& shapes, int start, int end) {
    Vec3 minExtents = Vec3(std::numeric_limits<float>::max());
    Vec3 maxExtents = Vec3(std::numeric_limits<float>::lowest());

    for (int i = start; i < end; ++i) {
        BoundingVolume shapeBounds = shapes[i]->getBoundingVolume();
        minExtents = minExtents.min(shapeBounds.minV);
        maxExtents = minExtents.min(shapeBounds.maxV);
    }

    return BoundingVolume(minExtents, maxExtents);
}

// Returns the largest axis of the bounding box
int BoundingVolume::getLargestAxis() const {
    Vec3 extents = maxV - minV;
    if (extents.x > extents.y && extents.x > extents.z) {
        return 0; // x-axis
    } else if (extents.y > extents.z) {
        return 1; // y-axis
    } else {
        return 2; // z-axis
    }
}

// Builds the BVH from a list of shapes
void BVH::build(const std::vector<std::shared_ptr<Shape>>& shapes) {
    // Create a copy of the shapes to sort them
    std::vector<std::shared_ptr<Shape>> shapesCopy = shapes;

    // Recursively build the BVH
    root = buildRecursive(shapesCopy, 0, shapesCopy.size());
}

// Recursive function to build the BVH
std::shared_ptr<BVHNode> BVH::buildRecursive(std::vector<std::shared_ptr<Shape>>& shapes, int start, int end) {
    auto node = std::make_shared<BVHNode>();

    // Compute the bounding volume for all shapes in this range
    node->bounds = BoundingVolume::computeBounds(shapes, start, end);

    int objectCount = end - start;
    if (objectCount <= 2) {
        // Leaf node: Store the shapes directly
        node->objects = std::vector<std::shared_ptr<Shape>>(shapes.begin() + start, shapes.begin() + end);
        node->isLeaf = true;
        return node;
    }

    // Internal node: Partition the shapes
    int mid = (start + end) / 2;

    // Sort by centroid along the largest axis of the bounding volume
    int splitAxis = node->bounds.getLargestAxis();
    std::nth_element(shapes.begin() + start, shapes.begin() + mid, shapes.begin() + end,
                     [splitAxis](const std::shared_ptr<Shape>& a, const std::shared_ptr<Shape>& b) {
                         return a->getCentroid()[splitAxis] < b->getCentroid()[splitAxis];
                     });

    // Recursively build child nodes
    node->left = buildRecursive(shapes, start, mid);
    node->right = buildRecursive(shapes, mid, end);
    node->isLeaf = false;

    return node;
}

// Traverses the BVH to find the closest intersection
bool BVH::traverse(const Ray& ray, Intersection& nearestIntersection) const {
    if (!root) return false; // No BVH built
    return traverseRecursive(root, ray, nearestIntersection);
}

// Recursive function to traverse the BVH
bool BVH::traverseRecursive(const std::shared_ptr<BVHNode>& node, const Ray& ray, Intersection& nearestIntersection) const {
    if (!node->bounds.intersects(ray)) {
        // Ray misses the bounding volume
        return false;
    }

    if (node->isLeaf) {
        // Test intersections with objects in the leaf
        bool hit = false;
        for (const auto& shape : node->objects) {
            Intersection tempIntersection;
            if (shape->intersect(ray, tempIntersection) && tempIntersection.t < nearestIntersection.t) {
                nearestIntersection = tempIntersection;
                hit = true;
            }
        }
        return hit;
    }

    // Recursively traverse child nodes
    bool hitLeft = traverseRecursive(node->left, ray, nearestIntersection);
    bool hitRight = traverseRecursive(node->right, ray, nearestIntersection);

    return hitLeft || hitRight;
}
