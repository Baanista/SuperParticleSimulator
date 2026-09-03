#pragma once
#include "../ParticleMatter.hpp"
#include <iostream>
#include <cmath>

class Cell : public ParticleMatter {
public:
    Cell(
        const sf::Vector2f& position,
        const sf::Vector2f& velocity,
        float radius,
        float mass);

    virtual ~Cell() noexcept = default;

    void update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* particleSystem) override;
    void draw(sf::RenderWindow& window) const override;

private:
    void duplicate(ParticleSystem* particleSystem);

    float atp_; // amount of energy
    sf::Angle angle_;
};