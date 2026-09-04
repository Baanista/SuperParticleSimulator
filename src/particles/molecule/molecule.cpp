#include "molecule.hpp"
#include "../../ParticleSystem.hpp"

const MoleculeProperties MoleculeProperties::WATER          = { MoleculeType::Water,        1.00f, sf::Color(0, 119, 182) };   // Deep Blue
const MoleculeProperties MoleculeProperties::CARBON_DIOXIDE = { MoleculeType::CarbonDioxide, 1.98f, sf::Color(108, 117, 125) };// Dark Gray
const MoleculeProperties MoleculeProperties::CARBON         = { MoleculeType::Carbon,        2.26f, sf::Color(33, 37, 41) };    // Charcoal
const MoleculeProperties MoleculeProperties::OXYGEN         = { MoleculeType::Oxygen,        0.80f, sf::Color(229, 56, 59) };   // Red
const MoleculeProperties MoleculeProperties::NITROGEN       = { MoleculeType::Nitrogen,      1.25f, sf::Color(114, 9, 183) };  // Purple
const MoleculeProperties MoleculeProperties::SUGAR          = { MoleculeType::Sugar,         1.59f, sf::Color(255, 200, 221) }; // Soft Pink
const MoleculeProperties MoleculeProperties::LIPID          = { MoleculeType::Lipid,         0.92f, sf::Color(255, 214, 10) };  // Yellow
const MoleculeProperties MoleculeProperties::PROTEIN        = { MoleculeType::Protein,       1.35f, sf::Color(255, 109, 0) };   // Orange
const MoleculeProperties MoleculeProperties::PHOSPHORUS     = { MoleculeType::Phosphorus,    1.82f, sf::Color(224, 170, 255) }; // Light Purple
const MoleculeProperties MoleculeProperties::PHOSPHOLIPID   = { MoleculeType::Phospholipid,  1.03f, sf::Color(82, 183, 136) };  // Teal-Green

const MoleculeProperties& MoleculeProperties::get(MoleculeType type) {
    switch (type) {
        case MoleculeType::Water:         return WATER;
        case MoleculeType::CarbonDioxide: return CARBON_DIOXIDE;
        case MoleculeType::Carbon:        return CARBON;
        case MoleculeType::Oxygen:        return OXYGEN;
        case MoleculeType::Nitrogen:      return NITROGEN;
        case MoleculeType::Sugar:         return SUGAR;
        case MoleculeType::Lipid:         return LIPID;
        case MoleculeType::Protein:       return PROTEIN;
        case MoleculeType::Phosphorus:    return PHOSPHORUS;
        case MoleculeType::Phospholipid:  return PHOSPHOLIPID;
        default:                          return WATER;
    }
};


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
    if (mass_ > maxMass)
    {
        split(system);
    }
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

void Molecule::split(ParticleSystem* system){
    mass_ *= 0.5;
    system->addParticle(std::make_shared<Molecule>(
        position_ + sf::Vector2f(1.0f, 0.0f),
        velocity_,
        properties_,
        mass_
    ));
};

bool Molecule::isAlive()
{
    return mass_ > 0;
}