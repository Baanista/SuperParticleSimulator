#pragma once
#include "../ParticleMatter.hpp"

enum MoleculeType : size_t
{
    Water,
    CarbonDioxide,
    Carbon,
    Oxygen,
    Nitrogen,
    Sugar,
    Lipid,
    Protein,
    Phosphorus,
    Phospholipid,
    COUNT
};

struct MoleculeProperties
{
    MoleculeType type;
    float density;
    sf::Color color_;

    static const MoleculeProperties WATER;
    static const MoleculeProperties CARBON_DIOXIDE;
    static const MoleculeProperties CARBON;
    static const MoleculeProperties OXYGEN;
    static const MoleculeProperties NITROGEN;
    static const MoleculeProperties SUGAR;
    static const MoleculeProperties LIPID;
    static const MoleculeProperties PROTEIN;
    static const MoleculeProperties PHOSPHORUS;
    static const MoleculeProperties PHOSPHOLIPID;

    MoleculeProperties(MoleculeType t, float d, sf::Color c)
        : type(t), density(d), color_(c) {}


    MoleculeProperties(MoleculeType t) {
        *this = get(t);
    }


    static const MoleculeProperties& get(MoleculeType type);
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
    void split(ParticleSystem* system);
    
    static constexpr float maxMass = 10.0f;
    MoleculeProperties getProperties(){return properties_; };
    MoleculeType getMoleculeType(){return properties_.type; }
    

private:
    MoleculeProperties properties_;
    void resetRadius(){radius_ = sqrt(mass_) / (properties_.density * (.3));};
    
};

template <typename T>
constexpr std::size_t to_idx(T e) {
    return static_cast<std::size_t>(e);
}
