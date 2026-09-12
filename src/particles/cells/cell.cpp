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
    age_ = 0;
    outerColor = sf::Color::White;
    dna_ = std::make_shared<DNA>(0);
    atp_ = starting_atp;
    color_ = sf::Color::Cyan;
    angle_ = sf::degrees(0);
    mutationAmount = .1f;
    cytoplasm_.add(Phospholipid, 10).add(CarbonDioxide, 10).add(Water, 10);
    radius_ = radius();
    lifetime_ = radius_ * radius_;
};

Cell::~Cell() noexcept
{
    ;
};

void Cell::onDeath(ParticleSystem* system) {

    
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
    radius_ = radius();
    atp_ -= (radius_ * radius_ * 0.001 + 0.01) * .1 * dt;



    mass_ = atp_ * .5 * 0.3 + cytoplasm_.mass;
    
    // limits the amount of atp produced
    atp_ = std::min({radius_ * radius_, atp_});

    setInputGenes().setCytoplasmInput();
    dna_->run();
    useOutputGenes(system, dt).manageCytoplasm();

    float cytoplasmSpace = getSpaceAmount();
    for (Particle* p : nearby) {
        if (p == this)
            continue;

        // interactions with molecules
        Molecule* molecule = dynamic_cast<Molecule*>(p);
        

        if (molecule)
        {
            if (isTouching(molecule, radius_ * 2) && molecule->getMass() > 0){
                if (managecytoplasm_[molecule->getMoleculeType()] > 0.0f){
                    absorb(molecule, molecule->getMoleculeType(), managecytoplasm_[molecule->getMoleculeType()] * dt);
                }
            }
        }
        Cell* cell = dynamic_cast<Cell*>(p);
        if (cell){
            if (cell->getSpaceAmount() * 5 < getSpaceAmount()){
                consume(cell);
            }
            if (getSpaceAmount() * 5 < cell->getSpaceAmount()){
                cell->consume(this);
            }
        }
    }

    for (size_t i = 0; i < MoleculeType::COUNT; i++)
    {
        if (managecytoplasm_[i] < -5)
        {
            auto type = static_cast<MoleculeType>(i);
            drop(system ,type, std::abs(managecytoplasm_[type]));
        }
    }
}

void Cell::consume(Cell* other){

}

Cell& Cell::drop(ParticleSystem* system, MoleculeType type, float amount){
    float aviableDropAmount = cytoplasm_[type];
    float dropAmount = std::min({amount, aviableDropAmount});

    sf::Vector2f offset(radius_ * 2 + static_cast<float>(type), angle_ + sf::degrees(180));
    sf::Vector2f newPos = position_ + offset;
    
    system->addParticle(std::make_shared<Molecule>(
        newPos,
        velocity_,
        type,            // molecule type
        dropAmount // mass of molecule
    ));

    cytoplasm_.remove(type, dropAmount);
    dna_->setValue(type, 0.0f);
    return *this;
}

Cell& Cell::dropExtraCytoplasm(ParticleSystem* system){
    float extraSpace = ; 

    return *this;
}

Cell& Cell::absorb(Molecule* molecule, MoleculeType type, float amount){
    if (molecule->getProperties().type != type) {return *this;};
    float cytoplasmSpace = getSpaceAmount();
    float leftSpaceMass = (cytoplasmSpace - cytoplasm_.space) / molecule->getProperties().density;
    float absorbAmount = std::min({amount, molecule->mass_, leftSpaceMass});
    cytoplasm_.add(type, absorbAmount);
    molecule->mass_ -= absorbAmount;
    managecytoplasm_.remove(type, absorbAmount);
    // velocity_ += (absorbAmount / mass_) * molecule->velocity_;
    return *this;
}

Cell& Cell::setInputGenes(){
    
    dna_->setValue(In_Genes::RADIUS, radius_);
    dna_->setValue(In_Genes::ATP,    atp_);
    float geneangle = angle_.asRadians();
    dna_->setValue(In_Genes::ANGLE,  geneangle);
    dna_->setValue(In_Genes::AGE, age_);
    dna_->setValue(In_Genes::LIFETIME, lifetime_);
    return (*this);
};

Cell& Cell::setCytoplasmInput(){
    for (size_t i = 0; i < MoleculeType::COUNT; i++){
        MoleculeType type = static_cast<MoleculeType>(i);
        dna_->setCytoplasmValue(type, cytoplasm_[type]);
    }
    return (*this);
};

Cell& Cell::useOutputGenes(ParticleSystem* system, float dt){
    auto clampColor = [](int value) {
        return static_cast<std::uint8_t>(std::clamp(value, 0, 255));
    };
    atp_ -= std::abs((*dna_)[Out_Genes::ROTATE] * 0.05);
    angle_ += sf::radians(dna_->resetValue(Out_Genes::ROTATE));
    photosynthesize(system, dna_->resetValue(Out_Genes::PHOTOSYNTHESIZE), dt);
    metabolize_sugar(dna_->resetValue(Out_Genes::METABOLIZE_SUGAR), dt);

    outerColor.r = clampColor(dna_->resetValue(Out_Genes::OUTRED));
    outerColor.b = clampColor(dna_->resetValue(Out_Genes::OUTBLUE));
    outerColor.g = clampColor(dna_->resetValue(Out_Genes::OUTGREEN));

    float dupeAmount = dna_->resetValue(Out_Genes::DUPLICATE);
    if (dupeAmount > 1 && age_ >= 5 && radius_ > 10){
        duplicate(system);
    }

    return (*this);
};

Cell& Cell::manageCytoplasm() {
    for (size_t type = 0; type < MoleculeType::COUNT; type++) {
        float geneVal = dna_->getGene(static_cast<MoleculeType>(type));
        managecytoplasm_[type] = geneVal;
        dna_->setValue(static_cast<MoleculeType>(type), std::min(geneVal, 0.0f));
    }
    return *this;
}

void Cell::consume(Cell* other){
    // for ()
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
    
    newCell->angle_ = angle_;

    // 1. Split cytoplasm resources between parent and offspring
    for (size_t i = 0; i < MoleculeType::COUNT; ++i)
    {
        newCell->cytoplasm_[i] = cytoplasm_[i] * otherNutrientPriority;
        cytoplasm_[i] *= (1.0f - otherNutrientPriority);
    }
    newCell->cytoplasm_.space *= otherNutrientPriority;
    cytoplasm_.space *= (1.0f - otherNutrientPriority);
    
    
    // 2. Generate a slightly mutated color based on the parent's color
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> delta(0, mutationAmount * 300); // Small tint variance

    auto clampColor = [](int value) {
        return static_cast<std::uint8_t>(std::clamp(value, 0, 255));
    };

    int colorChangeAmount = static_cast<int>(delta(gen));
    newCell->color_ = sf::Color(
        clampColor(color_.r + static_cast<int>(delta(gen) + .5)),
        clampColor(color_.g + static_cast<int>(delta(gen) + .5)),
        clampColor(color_.b + static_cast<int>(delta(gen) + .5)),
        color_.a
    );

    newCell->deathSystem = this->deathSystem;
    newCell->dna_ = dna_->mutate(mutationAmount);
    std::normal_distribution<float> mutationAmountGen(0.0f, mutationAmount);
    newCell->mutationAmount = std::min(mutationAmount + mutationAmountGen(gen), .005f);


    // 3. Register offspring particle system
    system->addParticle(newCell);
    return newCell;
}

void Cell::draw(sf::RenderWindow& window) const {
    sf::CircleShape shapeLabel(radius_ + 1); shapeLabel.setFillColor(sf::Color::White); shapeLabel.setPosition(position_ + sf::Vector2f(-radius_ - 3, -radius_ - 3));
    sf::CircleShape shape(radius_ * .9);
    shape.setFillColor(color_);

    shape.setOutlineThickness(radius_ * .1);
    shape.setOutlineColor(outerColor);

    shape.setPosition(position_ + sf::Vector2f(-radius_, -radius_));

    sf::RectangleShape angleLine({radius_ * 2, 2});
    angleLine.setPosition(position_);
    angleLine.rotate(angle_);
    angleLine.setFillColor(sf::Color::Red);
    
    // window.draw(shapeLabel);
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
    bool isalive = atp_ > 0 && lifetime_ > 0 && radius_ > 5;
    return isalive;
}