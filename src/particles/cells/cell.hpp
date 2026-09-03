#pragma once
#include "../ParticleMatter.hpp"
#include "../molecule/molecule.hpp"
#include "DNA.hpp"
#include <iostream>
#include <cmath>
#include <memory>

class Cell : public ParticleMatter {
public:
    Cell(
        const sf::Vector2f& position,
        const sf::Vector2f& velocity,
        float starting_atp);

    virtual ~Cell() noexcept = default;

    void update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system) override;
    void draw(sf::RenderWindow& window) const override;

private:
    std::shared_ptr<Cell> duplicate(ParticleSystem* system);

    float atp_; // amount of energy
    sf::Angle angle_;
    friend class DNA;
};