#pragma once
#include "GravityField.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
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
        if (particles_.size() + pendingParticles_.size() < maxParticles_) {
            pendingParticles_.push_back(std::make_shared<T>(std::forward<Args>(args)...));
        }
    }

    void emit(const sf::Vector2f& position, unsigned int count);
    void addParticle(std::shared_ptr<Particle> particle);
    void update(float dt);
    void draw(sf::RenderWindow& window);
    bool centerGravity = false;

private:
    std::vector<std::shared_ptr<Particle>> particles_;
    std::vector<std::shared_ptr<Particle>> pendingParticles_;
    unsigned int maxParticles_;
    SpatialGrid grid_;

    std::shared_ptr<GravityField> gravityField_;
};
