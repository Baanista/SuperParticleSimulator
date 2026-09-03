#include "cell.hpp"

Cell::Cell(
    const sf::Vector2f& position,
    const sf::Vector2f& velocity,
    float radius,
    float mass)
    : ParticleMatter(position, velocity, radius, mass)
{

};

void Cell::update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* particleSystem) {
    this->update(dt, nearby, particleSystem);

    // temporary way of optaining atp
    atp_ += 1;


    radius_ = sqrt(atp_);
    mass_ = atp_ * .5;
}


void Cell::draw(sf::RenderWindow& window) const {
   this->draw(window);
}