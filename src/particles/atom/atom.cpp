#include "atom.hpp"

Atom::Atom(
        const sf::Vector2f& position,
        const sf::Vector2f& velocity,
        float radius,
        float mass,
        sf::Color color,
        int defaultValence
    )
    : ParticleMatter(position, velocity, radius, mass),
    currentValence(defaultValence)
{
    this->color_ = color;

};
