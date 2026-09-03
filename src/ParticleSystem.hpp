#pragma once
#include "GravityField.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "particle_types.hpp"
#include "SpatialGrid.hpp"


class GravityField;

class ParticleSystem {
public:
    enum BorderBehavior {
        WRAP,
        BOUNCE,
        NONE
    };

    ParticleSystem(unsigned int maxParticles, sf::Vector2f size, BorderBehavior behavior = WRAP);

    bool nBodyGravity = false;
    float downwardGravity = 10.f;
    BorderBehavior borderBehavior = WRAP;
    sf::Vector2f size;

    template<typename T, typename... Args>
    void emitOne(Args&&... args) {
        if (particles_.size() < maxParticles_)
            particles_.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void emit(const sf::Vector2f& position, unsigned int count);
    void addParticle(std::unique_ptr<Particle> particle);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool centerGravity = false;

private:
    std::vector<std::unique_ptr<Particle>> particles_;
    unsigned int maxParticles_;
    SpatialGrid grid_;

    std::unique_ptr<GravityField> gravityField_;
};
