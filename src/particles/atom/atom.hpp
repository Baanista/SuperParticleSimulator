#pragma once
#include "../ParticleMatter.hpp"
#include <iostream>
#include <cmath>

class Atom : public ParticleMatter {
public:
    Atom(
        const sf::Vector2f& position,
        const sf::Vector2f& velocity,
        float radius,
        float mass,
        sf::Color color,
        int defaultValence

    )
    ;
    const static int maxValence = 8;
    const static int defaultValence;

    
private:
    int currentValence;
};
