#include "photon.h"

// Store photons temporarily
void PhotonMap::storePhoton(const Vec3& position, const Vec3& direction, const Colour& energy) {
    photons.push_back({position, direction, energy});
}

// Build the k-d tree from the stored photons
void PhotonMap::build() {
    root = buildKDTree(0, photons.size(), 0);
}

// Recursive function to build the k-d tree
std::unique_ptr<KDNode> PhotonMap::buildKDTree(int start, int end, int depth) {
    if (start >= end) return nullptr;

    // Determine the axis (0 = x, 1 = y, 2 = z)
    int axis = depth % 3;

    // Sort photons by the current axis
    auto comparator = [axis](const Photon& a, const Photon& b) {
        return a.position[axis] < b.position[axis];
    };
    std::sort(photons.begin() + start, photons.begin() + end, comparator);

    // Find the median and create the node
    int mid = (start + end) / 2;
    auto node = std::make_unique<KDNode>(photons[mid]);

    // Recursively build left and right subtrees
    node->left = buildKDTree(start, mid, depth + 1);
    node->right = buildKDTree(mid + 1, end, depth + 1);

    return node;
}

// Query photons within a radius
std::vector<Photon> PhotonMap::query(const Vec3& position, float radius) const {
    std::vector<Photon> result;
    queryKDTree(root.get(), position, radius, 0, result);
    return result;
}

void PhotonMap::printDebugInfo(size_t limit) const {
    std::cout << "Photon Map Debug: Total Photons Stored = " << photons.size() << std::endl;

    for (size_t i = 0; i < std::min(photons.size(), limit); ++i) {
        const Photon& photon = photons[i];
        std::cout << "Photon " << i << ": Position = (" << photon.position.x << ", " << photon.position.y << ", " << photon.position.z << ")"
              << ", Direction = (" << photon.direction.x << ", " << photon.direction.y << ", " << photon.direction.z << ")"
              << ", Energy = (" << photon.energy.r << ", " << photon.energy.g << ", " << photon.energy.b << ")" << std::endl;
    }
}

// Recursive function to query the k-d tree
void PhotonMap::queryKDTree(const KDNode* node, const Vec3& position, float radius, int depth, std::vector<Photon>& result) const {
    if (!node) return;

    // Check the current node
    float distSquared = (node->photon.position - position).length();
    distSquared = distSquared * distSquared;
    if (distSquared <= radius * radius) {
        result.push_back(node->photon);
    }

    // Determine the axis (0 = x, 1 = y, 2 = z)
    int axis = depth % 3;
    float delta = position[axis] - node->photon.position[axis];

    // Recursively query the relevant child nodes
    if (delta < 0) {
        queryKDTree(node->left.get(), position, radius, depth + 1, result);
        if (delta * delta <= radius * radius) {
            queryKDTree(node->right.get(), position, radius, depth + 1, result);
        }
    } else {
        queryKDTree(node->right.get(), position, radius, depth + 1, result);
        if (delta * delta <= radius * radius) {
            queryKDTree(node->left.get(), position, radius, depth + 1, result);
        }
    }
}
