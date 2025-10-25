#pragma once
#include <unordered_map>
#include <vector>
#include <SFML/System/Vector2.hpp>

class Particle; // forward declaration

class SpatialGrid {
public:
    explicit SpatialGrid(float cellSize = 8.f);

    void clear();
    void insert(Particle* particle, const sf::Vector2f& position);
    std::vector<Particle*> queryRange(const sf::Vector2f& position, float range) const;

private:
    float cellSize_;
    std::unordered_map<long long, std::vector<Particle*>> cells_;

    static long long hashCell(int x, int y);
};
