#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <vector>
#include "particles/ParticleMatter.hpp"

class GravityField {
public:
    GravityField(float width, float height, float theta, float G);

    void build(const std::vector<ParticleMatter*>& particles);
    sf::Vector2f computeForceOn(const ParticleMatter& particle) const;

private:
    struct Node {
        sf::Rect<float> bounds;
        sf::Vector2f centerOfMass{0.f, 0.f};
        float totalMass = 0.f;
        bool isLeaf = true;
        std::vector<ParticleMatter*> particles;

        std::unique_ptr<Node> nw, ne, sw, se;

        bool contains(const sf::Vector2f& pos) const {
            return bounds.contains(pos);
        }
    };

    void insert(Node& node, ParticleMatter* p);
    void computeCenterOfMass(Node& node);
    sf::Vector2f computeForce(const Node& node, const ParticleMatter& target) const;

    float width_, height_;
    float theta_;
    float G_;
    std::unique_ptr<Node> root_;
};
