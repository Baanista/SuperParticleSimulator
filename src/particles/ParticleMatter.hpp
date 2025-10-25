#pragma once
#include "../Particle.hpp"
#include <cmath>

class ParticleMatter : public Particle {
public:
    ParticleMatter(
        const sf::Vector2f& position,
        const sf::Vector2f& velocity,
        float radius,
        float mass
    )
    ;

    void update(float dt, const std::vector<Particle*>& nearby) override;

    float getMass() const { return mass_; }
    void setMass(float m) { mass_ = m; }

protected:
    void resolveCollision(ParticleMatter* other, float collisionDamp);
    void circle_collision_result(double distance, double ox, double oy, double other_velocityx, double other_velocityy, double other_mass, double our_size, float collisionDamp);

private:
    float mass_;
};
