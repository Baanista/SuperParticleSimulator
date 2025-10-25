#include "SpatialGrid.hpp"
#include "particle_types.hpp"
#include <cmath>

SpatialGrid::SpatialGrid(float cellSize)
    : cellSize_(cellSize) {}

void SpatialGrid::clear() {
    cells_.clear();
}

long long SpatialGrid::hashCell(int x, int y) {
    return (static_cast<long long>(x) << 32) ^ static_cast<unsigned long long>(y);
}

void SpatialGrid::insert(Particle* particle, const sf::Vector2f& position) {
    int gx = static_cast<int>(std::floor(position.x / cellSize_));
    int gy = static_cast<int>(std::floor(position.y / cellSize_));
    cells_[hashCell(gx, gy)].push_back(particle);
}

std::vector<Particle*> SpatialGrid::queryRange(const sf::Vector2f& position, float range) const {
    std::vector<Particle*> found;

    // Compute which grid cells overlap with the query circle
    int minX = static_cast<int>(std::floor((position.x - range) / cellSize_));
    int maxX = static_cast<int>(std::floor((position.x + range) / cellSize_));
    int minY = static_cast<int>(std::floor((position.y - range) / cellSize_));
    int maxY = static_cast<int>(std::floor((position.y + range) / cellSize_));

    float rangeSq = range * range;

    // Loop over all relevant cells
    for (int gx = minX; gx <= maxX; ++gx) {
        for (int gy = minY; gy <= maxY; ++gy) {
            auto it = cells_.find(hashCell(gx, gy));
            if (it == cells_.end()) continue;

            // Loop over particles in the cell and check actual distance
            for (Particle* p : it->second) {
                const sf::Vector2f& pos = p->getPosition();
                float dx = pos.x - position.x;
                float dy = pos.y - position.y;
                if (dx * dx + dy * dy <= rangeSq)
                    found.push_back(p);
            }
        }
    }

    return found;
}
