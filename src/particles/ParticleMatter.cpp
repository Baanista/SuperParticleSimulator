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

void ParticleMatter::update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system) {
    // Apply motion
    position_ += velocity_ * dt;
    //velocity_ *= 0.99f; // simple damping to prevent infinite acceleration
    lifetime_ -= dt;

    // Handle collisions with nearby particles
    for (Particle* p : nearby) {
        if (p == this)
            continue;

        // Only collide with other ParticleMatter objects
        auto* other = dynamic_cast<ParticleMatter*>(p);
        if (!other)
            continue;

        nearAddVelocity(other, -10 * dt, detectionRange_ * .5);
        // nearAddVelocity(other, 10 * dt, detectionRange_ );
        // nearAddVelocity(other, 20 * dt, detectionRange_ * .8);
        // nearAddVelocity(other, -40 * dt, detectionRange_ * .8);
        // other->nearAddVelocity(this, 7 * dt, detectionRange_ * .4);
    }

    for (Particle* p : nearby) {
        if (p == this)
            continue;

        // Only collide with other ParticleMatter objects
        auto* other = dynamic_cast<ParticleMatter*>(p);
        if (!other)
            continue;

        resolveCollision(other, .9f);
    }
    applyConnectionForces(dt);
}

void ParticleMatter::addConnection(ParticleMatter* other) {
    if (!other || other == this) return;

    // Prevent duplicate entries
    auto it = std::find(connections_.begin(), connections_.end(), other);
    if (it == connections_.end()) {
        connections_.push_back(other);
        other->connections_.push_back(this); // Keep bi-directional
    }
}

void ParticleMatter::removeConnection(ParticleMatter* other) {
    auto it = std::find(connections_.begin(), connections_.end(), other);
    if (it != connections_.end()) {
        connections_.erase(it);
    }
}

void ParticleMatter::breakAllConnections() {
    for (ParticleMatter* other : connections_) {
        if (other) {
            other->removeConnection(this);
        }
    }
    connections_.clear();
}

void ParticleMatter::applyConnectionForces(float dt) {
    for (ParticleMatter* other : connections_) {
        sf::Vector2f delta = other->getPosition() - position_;
        float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

        if (dist > 0.0001f) {
            sf::Vector2f dir = delta / dist;
            float restLength = radius_ + other->radius_;
            float displacement = dist - restLength;

            // Spring force calculation (F = k * dx)
            sf::Vector2f force = dir * (displacement * springStiffness_);
            velocity_ += (force / mass_) * dt;
        }
    }
}


void ParticleMatter::drawLinks(sf::RenderWindow& window) const {
    std::vector<sf::Vertex> lineVertices;

    for (const ParticleMatter* other : connections_) {
        if (other < this) continue; // Draw each connection once

        sf::Vector2f posA = position_;
        sf::Vector2f posB = other->getPosition();

        sf::Vector2f delta = posB - posA;
        float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

        // Distance-based alpha fade
        float fadeFactor = 1.0f - (dist / maxBondDistance_);
        auto alpha = static_cast<std::uint8_t>(std::clamp(fadeFactor, 0.0f, 1.0f) * 255.0f);

        sf::Color lineCol{255, 255, 255, alpha};

        // SFML 3 Vertex constructor syntax
        lineVertices.push_back(sf::Vertex{posA, lineCol});
        lineVertices.push_back(sf::Vertex{posB, lineCol});
    }

    if (!lineVertices.empty()) {
        // SFML 3 window.draw call
        window.draw(lineVertices.data(), lineVertices.size(), sf::PrimitiveType::Lines);
    }
}

void ParticleMatter::draw(sf::RenderWindow& window) const{
    Particle::draw(window);
    drawLinks(window);
}


void ParticleMatter::resolveCollision(ParticleMatter* other, float collisionDamp) {

    sf::Vector2f delta = other->getPosition() - position_;
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    float minDist = radius_ + other->radius_;

    // calculate the unit normal and tangential vectors
    if (distance < minDist && distance > 0.01f) {
        
        double attractiontemp = (minDist - distance) * 0.5;

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