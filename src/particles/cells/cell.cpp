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
    lifetime_ = 120;
    cytoplasm_[Phospholipid] = 10;
};

Cell::~Cell() noexcept
{
    ;
};

void Cell::onDeath(ParticleSystem* system) {

    if (!radius_)
    {
        std::cout << "invalid cell" << std::endl;
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

            
            system->addParticle(std::make_shared<Molecule>(
                newPos,
                velocity_,
                type,            // molecule type
                cytoplasm_[i] // mass of molecule
            ));
        }
    }
}

void Cell::update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system) {
    ParticleMatter::update(dt, nearby, system);
    deathSystem = system;
    
    if (!radius_)
    {
        std::cout << "invalid cell" << std::endl;
    }

    // temporary way of optaining atp
    lifetime_ -= dt;
    metabolize_sugar(radius_ * 0.1 * dt);


    photosynthesize(10.0f * dt);

    atp_ -= (radius_ * radius_ * 0.001 + 0.01) * .1 * dt;
    

    radius_ = std::min({cytoplasm_[Phospholipid] * 5, 20.0f});
    mass_ = atp_ * .5 * 0.3;
    angle_ += sf::degrees(1);
    // limits the amount of atp produced
    atp_ = std::min({radius_ * radius_, atp_});

    if (cytoplasm_[Phospholipid] > 5 && age_ > 10)
    {
        duplicate(system);
    }

    for (Particle* p : nearby) {
        if (p == this)
            continue;

        // interactions with molecules
        Molecule* molecule = dynamic_cast<Molecule*>(p);
        if (molecule)
        {
            if (isTouching(molecule) && molecule->getMass() > 0){
                // float added_mass = ;
                cytoplasm_[molecule->getMoleculeType()] += molecule->getMass();
                molecule->setMass(0);
            }
        }

        
    }
}

std::shared_ptr<Cell> Cell::duplicate(ParticleSystem* system)
{
    float otherNutrientPriority = 0.5f;

    sf::Vector2f new_pos = position_ + sf::Vector2f(radius_ * 2.0f, 0.0f);
    atp_ *= .5;
    std::shared_ptr<Cell> newCell = std::make_shared<Cell>(
        new_pos,
        velocity_,
        atp_
    );

    // 1. Split cytoplasm resources between parent and offspring
    for (size_t i = 0; i < MoleculeType::COUNT; ++i)
    {
        newCell->cytoplasm_[i] = cytoplasm_[i] * otherNutrientPriority;
        cytoplasm_[i] *= (1.0f - otherNutrientPriority);
    }
    
    // 2. Generate a slightly mutated color based on the parent's color
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> delta(-20, 20); // Small tint variance

    auto clampColor = [](int value) {
        return static_cast<std::uint8_t>(std::clamp(value, 0, 255));
    };

    newCell->color_ = sf::Color(
        clampColor(color_.r + delta(gen)),
        clampColor(color_.g + delta(gen)),
        clampColor(color_.b + delta(gen)),
        color_.a
    );

    newCell->deathSystem = this->deathSystem;

    // 3. Register offspring particle system
    if (system) {
        system->addParticle(newCell);
    }
    else{
        std::cout << "system invalid" << std::endl;
    }

    if (!newCell)
    {
        std::cout << "new cell is invalid" << std::endl;
    }

    return newCell;
}

void Cell::draw(sf::RenderWindow& window) const {
   sf::CircleShape shape(radius_ * .9);
    shape.setFillColor(color_);

    shape.setOutlineThickness(radius_ * .1);
    shape.setOutlineColor(sf::Color(255, 255, 255));

    shape.setPosition(position_ + sf::Vector2f(-radius_, -radius_));

    sf::RectangleShape angleLine({radius_ * 2, 2});
    angleLine.setPosition(position_);
    angleLine.rotate(angle_);
    angleLine.setFillColor(sf::Color::Red);
    

    window.draw(shape);
    window.draw(angleLine);
}

Cell& Cell::kill()
{
    lifetime_ = 0;
    atp_ = -1000;
    return *this;
}

bool Cell::isAlive()
{
    bool isalive = atp_ > 0 && lifetime_ > 0;
    if (isalive)
    {
        ;
    }
    else{
        std::cout << "cell died" << std::endl;
    }
    return isalive;
}