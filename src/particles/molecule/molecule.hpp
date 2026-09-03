#pragma once
#include "../ParticleMatter.hpp"

enum MoleculeType
{
    Water,
    CarbonDioxide,
    Oxygen,
    Sugar
};

struct MoleculeProperties
{
    MoleculeType type;
    float density;
    sf::Color color_;

    static const MoleculeProperties Water;
    static const MoleculeProperties CarbonDioxide;
    static const MoleculeProperties Oxygen;
    static const MoleculeProperties Sugar;
};


//Will have the recombining feature so that 
class Molecule : public ParticleMatter
{
public:
    Molecule(    
    const sf::Vector2f& position,
    const sf::Vector2f& velocity,
    const MoleculeProperties& props,
    float mass);
    
    Molecule(    
    const sf::Vector2f& position,
    const sf::Vector2f& velocity,
    const MoleculeProperties& props)  : Molecule(position, velocity, props, maxMass){};

    virtual ~Molecule() noexcept = default;

    void update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system) override;

    bool isAlive() override;

    void absorb(Molecule* other);
    static constexpr float maxMass = 10.0f;
    
    

private:
    MoleculeProperties properties_;
    void resetRadius(){radius_ = sqrt(mass_) / properties_.density;};
    
};