#include "GravityField.hpp"
#include <cmath>

GravityField::GravityField(float width, float height, float theta, float G)
    : width_(width), height_(height), theta_(theta), G_(G) {
    root_ = std::make_unique<Node>();
    root_->bounds = sf::Rect<float>(sf::Vector2f{0.f, 0.f}, sf::Vector2f{width_, height_});
}

void GravityField::build(const std::vector<ParticleMatter*>& particles) {
    root_ = std::make_unique<Node>();
    root_->bounds = sf::Rect<float>(sf::Vector2f{0.f, 0.f}, sf::Vector2f{width_, height_});
    for (auto* p : particles)
        insert(*root_, p);
    computeCenterOfMass(*root_);
}

void GravityField::insert(Node& node, ParticleMatter* p) {
    if (!node.contains(p->getPosition()))
        return;

    if (node.isLeaf && node.particles.size() < 1) {
        node.particles.push_back(p);
        return;
    }

    // Subdivide if leaf
    if (node.isLeaf) {
        node.isLeaf = false;
        float x = node.bounds.position.x;
        float y = node.bounds.position.y;
        float w = node.bounds.size.x / 2.f;
        float h = node.bounds.size.y / 2.f;

        node.nw = std::make_unique<Node>();
        node.ne = std::make_unique<Node>();
        node.sw = std::make_unique<Node>();
        node.se = std::make_unique<Node>();

        node.nw->bounds = sf::Rect<float>(sf::Vector2f{x, y}, sf::Vector2f{w, h});
        node.ne->bounds = sf::Rect<float>(sf::Vector2f{x + w, y}, sf::Vector2f{w, h});
        node.sw->bounds = sf::Rect<float>(sf::Vector2f{x, y + h}, sf::Vector2f{w, h});
        node.se->bounds = sf::Rect<float>(sf::Vector2f{x + w, y + h}, sf::Vector2f{w, h});

        // Move existing particle to correct child
        auto old = node.particles.front();
        node.particles.clear();
        insert(*node.nw, old);
        insert(*node.ne, old);
        insert(*node.sw, old);
        insert(*node.se, old);
    }

    insert(*node.nw, p);
    insert(*node.ne, p);
    insert(*node.sw, p);
    insert(*node.se, p);
}

void GravityField::computeCenterOfMass(Node& node) {
    if (node.isLeaf) {
        if (!node.particles.empty()) {
            auto* p = node.particles.front();
            node.centerOfMass = p->getPosition();
            node.totalMass = p->getMass();
        }
        return;
    }

    sf::Vector2f weightedPos{0.f, 0.f};
    float totalMass = 0.f;

    auto accumulate = [&](const std::unique_ptr<Node>& n) {
        if (!n) return;
        computeCenterOfMass(*n);
        weightedPos += n->centerOfMass * n->totalMass;
        totalMass += n->totalMass;
    };

    accumulate(node.nw);
    accumulate(node.ne);
    accumulate(node.sw);
    accumulate(node.se);

    if (totalMass > 0.f)
        node.centerOfMass = weightedPos / totalMass;
    node.totalMass = totalMass;
}

sf::Vector2f GravityField::computeForceOn(const ParticleMatter& particle) const {
    if (!root_)
        return {0.f, 0.f};

    return computeForce(*root_, particle);
}

sf::Vector2f GravityField::computeForce(const Node& node, const ParticleMatter& target) const {
    if (node.totalMass <= 0.f)
        return {0.f, 0.f};

    // Vector from target to this node’s center of mass
    sf::Vector2f diff = node.centerOfMass - target.getPosition();
    float dist2 = diff.x * diff.x + diff.y * diff.y;

    // ✅ Softening (avoids infinite forces at small distances)
    const float softening = 25.0f; // tweak this; acts like a min distance squared
    dist2 += softening;

    float dist = std::sqrt(dist2);
    if (dist < 1e-3f)
        return {0.f, 0.f};

    float s = node.bounds.size.x; // size of region

    // ✅ Barnes–Hut opening criterion
    if (node.isLeaf || (s / dist) < theta_) {
        float forceMag = G_ * target.getMass() * node.totalMass / dist2;

        // ✅ Clamp extremely large forces
        const float maxForce = 500.0f;
        if (forceMag > maxForce)
            forceMag = maxForce;

        return (diff / dist) * forceMag;
    }

    // ✅ Recursively accumulate force from child nodes
    sf::Vector2f total{0.f, 0.f};
    auto accumulate = [&](const std::unique_ptr<Node>& n) {
        if (n)
            total += computeForce(*n, target);
    };

    accumulate(node.nw);
    accumulate(node.ne);
    accumulate(node.sw);
    accumulate(node.se);

    return total;
}

