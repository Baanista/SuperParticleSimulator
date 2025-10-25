#include "ParticleMatter.hpp"
#include <SFML/Graphics.hpp>

ParticleMatter::ParticleMatter(
    const sf::Vector2f& position,
    const sf::Vector2f& velocity,
    float radius,
    float mass
)
    : Particle(position, velocity, radius, 100000000000),
      mass_(mass)
{
    color_ = sf::Color::Yellow; // distinct color for matter particles
}

void ParticleMatter::update(float dt, const std::vector<Particle*>& nearby) {
    // Apply motion
    position_ += velocity_ * dt;
    lifetime_ -= dt;

    // Handle collisions with nearby particles
    for (Particle* p : nearby) {
        if (p == this)
            continue;

        // Only collide with other ParticleMatter objects
        auto* other = dynamic_cast<ParticleMatter*>(p);
        if (!other)
            continue;

        resolveCollision(other, 0.95);

        // nearAddVelocity(other, -8 * dt, detectionRange_ * .3);
        // nearAddVelocity(other, -15 * dt, detectionRange_ * .8);
        // other->nearAddVelocity(this, -4 * dt, detectionRange_ * .6);
        // other->nearAddVelocity(this, 7 * dt, detectionRange_ * .4);
    }
}

void ParticleMatter::resolveCollision(ParticleMatter* other, float collisionDamp) {

    sf::Vector2f delta = other->getPosition() - position_;
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    float minDist = radius_ + other->radius_;

    // calculate the unit normal and tangential vectors
    if (distance < minDist && distance > 0.01f) {
        
        double attractiontemp = (minDist - distance) * 0.25;

        sf::Vector2f this_velocity_ = velocity_;
        circle_collision_result(distance, other->getPosition().x, other->getPosition().y, other->velocity_.x, other->velocity_.y, other->getMass(), mass_, collisionDamp);
        other->circle_collision_result(distance, position_.x, position_.y, this_velocity_.x, this_velocity_.y, mass_, other->getMass(), collisionDamp);

        move(other, attractiontemp);
        other->move(this, attractiontemp);

    }
}

void ParticleMatter::circle_collision_result(double distance, double ox, double oy, double other_velocityx, double other_velocityy, double other_mass, double our_size, float collisionDamp)
{
    float vx = velocity_.x;
    float vy = velocity_.y;

    double mass = our_size;

    // calculate the unit normal and tangential vectors
    double unx = (ox - position_.x) / distance;
    double uny = (oy - position_.y) / distance;
    double utx = -uny;
    double uty = unx;

    // calculate the scalar normal and tangential velocities
    double v1n = vx * unx + vy * uny;
    double v1t = vx * utx + vy * uty;
    double v2n = other_velocityx * unx + other_velocityy * uny;
    double v2t = other_velocityx * utx + other_velocityy * uty;

    // calculate the new scalar normal velocities
    double v1n_new = (v1n * (mass - other_mass) + 2 * other_mass * v2n) / (mass + other_mass);
    double v2n_new = (v2n * (other_mass - mass) + 2 * mass * v1n) / (mass + other_mass);

    // calculate the new vector velocities
    double v1x_new = v1n_new * unx + v1t * utx;
    double v1y_new = v1n_new * uny + v1t * uty;
    double v2x_new = v2n_new * unx + v2t * utx;
    double v2y_new = v2n_new * uny + v2t * uty;
    
    
    // assign the new velocities to the global variables

    velocity_.x += (v1x_new - velocity_.x) * collisionDamp;
    velocity_.y += (v1y_new - velocity_.y) * collisionDamp;

}