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
    radius_ = 10.0f;
    lifetime_ = 10000;
};

Cell::~Cell() noexcept
{
    die();
};

void Cell::die()
{
    if (!deathSystem)
    {
        return;
    }
    radius_ = 10;
    
    // cell dies and emmits all of its contents from the cytoplasm
    for (size_t i = 0; i < MoleculeType::COUNT; i++)
    {
        if (cytoplasm_[i] > 0)
        {
            
            auto type = static_cast<MoleculeType>(i);
            sf::Angle angle = sf::degrees((360 / MoleculeType::COUNT) * i);
            sf::Vector2f offset(radius_, angle);
            sf::Vector2f newPos = position_ + offset;

            
            deathSystem->emitOne<Molecule>(
                newPos,
                velocity_,
                type,            // molecule type
                cytoplasm_[i] // mass of molecule
            );
        }
    }
};

void Cell::update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system) {
    deathSystem = system;
    ParticleMatter::update(dt, nearby, system);

    // temporary way of optaining atp
    lifetime_ --;
    metabolize_sugar(radius_);

    float photoAmout = (1 / (system->size.y - position_.y));
    // photosynthesize(photoAmout);

    atp_ -= (radius_ * radius_ * 0.001 + 0.01) * 5;


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
    float otherNutrientProirty = 0.5f;

    sf::Vector2f new_pos = position_ + sf::Vector2f(radius_ * 2, angle_);
    std::shared_ptr<Cell> newCell = std::make_shared<Cell>(
        new_pos,
        velocity_,
        atp_
    );

    for (int i; i < MoleculeType::COUNT; i++)
    {
        newCell->cytoplasm_[i] = cytoplasm_[i] * otherNutrientProirty;
        cytoplasm_[i] *= 1 - otherNutrientProirty;
    }
    
    system->addParticle(newCell);

    return newCell;
}

void Cell::draw(sf::RenderWindow& window) const {
   ParticleMatter::draw(window);
}

bool Cell::isAlive()
{
    return atp_ > 0 || lifetime_ < 0;
}