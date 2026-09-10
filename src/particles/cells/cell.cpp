#include "cell.hpp"
#include "../../ParticleSystem.hpp"

Cell::Cell(
    const sf::Vector2f& position,
    const sf::Vector2f& velocity,
    float starting_atp)
    : ParticleMatter(position, velocity, 0, 100),
      cytoplasm_(),
      managecytoplasm_()
{
    dna_ = std::make_shared<DNA>(0);
    atp_ = starting_atp;
    color_ = sf::Color::Cyan;
    angle_ = sf::degrees(0);
    radius_ = 10.0f;
    lifetime_ = 120;
    mutationAmount = .1;
    cytoplasm_[Phospholipid] = 10;
};

Cell::~Cell() noexcept
{
    ;
};

void Cell::onDeath(ParticleSystem* system) {


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

    ParticleMatter::update(dt, nearby, system); //issue here 
    deathSystem = system;
    


    // temporary way of optaining atp
    lifetime_ -= dt;

    atp_ -= (radius_ * radius_ * 0.001 + 0.01) * .1 * dt;



    radius_ = std::min({cytoplasm_[Phospholipid] * 5, 20.0f});
    mass_ = atp_ * .5 * 0.3;
    
    // limits the amount of atp produced
    atp_ = std::min({radius_ * radius_, atp_});

    dna_->run();
    setInputGenes().setCytoplasmInput().useOutputGenes().manageCytoplasm();

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

Cell& Cell::setInputGenes(){
    dna_->setValue(In_Genes::RADIUS, radius_);
    dna_->setValue(In_Genes::ATP,    atp_);
    float geneangle = angle_.asRadians();
    dna_->setValue(In_Genes::ANGLE,  geneangle);
    return (*this);
};

Cell& Cell::setCytoplasmInput(){
    for (size_t type = 0; type < MoleculeType::COUNT; type++){
        dna_->setCytoplasmValue(type, cytoplasm_[type]);
    }
    return (*this);
};

Cell& Cell::useOutputGenes(){
    angle_ += sf::radians(dna_->resetValue(Out_Genes::ROTATE));
    photosynthesize(dna_->resetValue(Out_Genes::PHOTOSYNTHESIZE));
    metabolize_sugar(dna_->resetValue(Out_Genes::METABOLIZE_SUGAR));
    return (*this);
};

Cell& Cell::manageCytoplasm() {
    for (size_t type = 0; type < MoleculeType::COUNT; type++) {
        float geneVal = dna_->getCytoplasmGene(type);
        managecytoplasm_[type] = geneVal;
        dna_->setCytoplasmValue(type, 0.0f);
    }
    return *this;
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
    std::normal_distribution<float> delta(0, mutationAmount * 10); // Small tint variance

    auto clampColor = [](int value) {
        return static_cast<std::uint8_t>(std::clamp(value, 0, 255));
    };

    newCell->color_ = sf::Color(
        clampColor(color_.r + static_cast<int>(delta(gen))),
        clampColor(color_.g + static_cast<int>(delta(gen))),
        clampColor(color_.b + static_cast<int>(delta(gen))),
        color_.a
    );

    newCell->deathSystem = this->deathSystem;
    newCell->dna_ = dna_->mutate(mutationAmount);
    std::normal_distribution<float> mutationAmountGen(0.0f, mutationAmount);
    newCell->mutationAmount = mutationAmount + mutationAmountGen(gen);

    // 3. Register offspring particle system
    system->addParticle(newCell);

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