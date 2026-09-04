#include "molecule.hpp"

const MoleculeProperties MoleculeProperties::Water         = { MoleculeType::Water, 0.3f, sf::Color::Blue };
const MoleculeProperties MoleculeProperties::CarbonDioxide = { MoleculeType::CarbonDioxide, 1.98f, sf::Color(128, 128, 128) };
const MoleculeProperties MoleculeProperties::Oxygen        = { MoleculeType::Oxygen, 1.43f, sf::Color::Red };
const MoleculeProperties MoleculeProperties::Sugar         = { MoleculeType::Sugar, .4f, sf::Color::White };

Molecule::Molecule(    
    const sf::Vector2f& position,
    const sf::Vector2f& velocity,
    const MoleculeProperties& props,
    float mass)
    : ParticleMatter(position, velocity, 1, mass),
    properties_(props)
{
    resetRadius();
    color_ = properties_.color_;
};

void Molecule::update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system)
{
    for (Particle* p : nearby)
    {
        if (p == this)
            continue;

        // Only collide with other ParticleMatter objects
        auto* other = dynamic_cast<Molecule*>(p);
        if (!other)
            continue;

        if (isTouching(other) && properties_.type == other->properties_.type)
        {
            if (other->mass_ < mass_)
            {
                absorb(other);
            }
            else{
                other->absorb(this);
            }
        }

    }

    ParticleMatter::update(dt, nearby, system);
};
/**
 * Takes as much mass from the other particle. Can make the other particles mass to 0
 */
void Molecule::absorb(Molecule* other) {
    if (!other || other->mass_ <= 0.0f) return;


    float availableCapacity = maxMass - mass_;
    if (availableCapacity <= 0.0f) return;


    float massToTake = std::min(other->mass_, availableCapacity);

    mass_ += massToTake;
    other->mass_ -= massToTake;


    resetRadius();
    other->resetRadius();
}

bool Molecule::isAlive()
{
    return mass_ > 0;
}