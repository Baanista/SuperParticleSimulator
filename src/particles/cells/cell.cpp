#include "cell.hpp"
#include "../../ParticleSystem.hpp"

Cell::Cell(
    const sf::Vector2f& position,
    const sf::Vector2f& velocity,
    float starting_atp)
    : ParticleMatter(position, velocity, 0, 100)
{
    atp_ = starting_atp;
    color_ = sf::Color::Cyan;
    angle_ = sf::degrees(0);
};

void Cell::update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system) {
    ParticleMatter::update(dt, nearby, system);

    // temporary way of optaining atp
    atp_ += (cytoplasm_[MoleculeType::Sugar] > 1) ? 1 : 0;


    radius_ = sqrt(atp_) * 0.3;
    mass_ = atp_ * .5 * 0.3;
    angle_ += sf::degrees(1);

    if (atp_ > 500)
    {
        atp_ *= .5;
       duplicate(system);
    }

    for (Particle* p : nearby) {
        if (p == this)
            continue;

        // interactions with molecules
        Molecule* molecule = dynamic_cast<Molecule*>(p);
        if (molecule)
        {
            if (isTouching(molecule)){
                cytoplasm_[molecule->getMoleculeType()] += molecule->getMass();
                molecule->setMass(0);
            }
        }

        
    }
}

std::shared_ptr<Cell> Cell::duplicate(ParticleSystem* system)
{
    sf::Vector2f new_pos = position_ + sf::Vector2f(radius_ * 2, angle_);
    std::shared_ptr<Cell> newCell = std::make_shared<Cell>(
        new_pos,
        velocity_,
        atp_
    );
    
    system->addParticle(newCell);

    return newCell;
}

void Cell::draw(sf::RenderWindow& window) const {
   ParticleMatter::draw(window);
}