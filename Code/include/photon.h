#ifndef PHOTON_H
#define PHOTON_H

#include "common.h"
#include <vector>
#include <algorithm>
#include <memory>

struct Photon {
    Vec3 position;      // Photon position
    Vec3 direction;     // Incoming direction of the photon
    Colour energy;      // Energy (colour) carried by the photon
};

// K-d Tree Node
struct KDNode {
    Photon photon;                  // Photon stored in this node
    std::unique_ptr<KDNode> left;   // Left child
    std::unique_ptr<KDNode> right;  // Right child

    KDNode(const Photon& p) : photon(p), left(nullptr), right(nullptr) {}
};

class PhotonMap {
public:
    PhotonMap() : root(nullptr) {}

    // Add a photon to the map
    void storePhoton(const Vec3& position, const Vec3& direction, const Colour& energy);

    // Build the k-d tree after adding all photons
    void build();

    // Query photons within a radius
    std::vector<Photon> query(const Vec3& position, float radius) const;

    // Debugging function to print photons
    void printDebugInfo(size_t limit = 100) const;

private:
    std::vector<Photon> photons;    // Temporary storage for photons before building the k-d tree
    std::unique_ptr<KDNode> root;   // Root of the k-d tree

    // Helper functions
    std::unique_ptr<KDNode> buildKDTree(int start, int end, int depth);
    void queryKDTree(const KDNode* node, const Vec3& position, float radius, int depth, std::vector<Photon>& result) const;
};

#endif // PHOTON_H
