#pragma once
#include "../ParticleMatter.hpp"
#include "../molecule/molecule.hpp"
#include "cytoplasm.hpp"
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

    virtual ~Cell() noexcept;
    void onDeath(ParticleSystem* system);

    ParticleSystem* deathSystem;

    void update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system) override;
    void draw(sf::RenderWindow& window) const override;
    bool isAlive() override;
    void die();

    Cell& kill();
    Cell& drop(ParticleSystem* system, MoleculeType type, float amount);
    Cell& absorb(Molecule* molecule, MoleculeType type, float amount);
    

private:
    sf::Color outerColor;
    std::shared_ptr<Cell> duplicate(ParticleSystem* system);

    float atp_; // amount of energy
    sf::Angle angle_;
    
    float getSpaceAmount() {return radius_ * radius_;};
    Cytoplasm cytoplasm_;

    // reseted every tick. Postivie values means the molocles that are wanted to absorbe. Negative is the amout that is wanted to drop
    Cytoplasm managecytoplasm_;

    // gene use
    std::shared_ptr<DNA> dna_;
    Cell& setInputGenes();
    Cell& setCytoplasmInput();
    Cell& useOutputGenes();
    Cell& manageCytoplasm();
    float mutationAmount;
    
    //atp_used it the amout of atp used per amount
    float convert_molecule(Cytoplasm in, Cytoplasm out, float amount, float atp_change);
    
    /* 
    cytoplasm manipilation
    takes in the amout of mass that wants to be made 
    returns the remaining amout that could not be converted
    */
    
    float metabolize_sugar(float amout);
    float photosynthesize(float amount);
    
};