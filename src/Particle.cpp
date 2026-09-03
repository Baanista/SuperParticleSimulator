#include "Particle.hpp"
#include "ParticleSystem.hpp"

Particle::Particle(const sf::Vector2f& position, const sf::Vector2f& velocity, float radius, float lifetime)
    : position_(position), velocity_(velocity), radius_(radius), lifetime_(lifetime), color_(sf::Color::White)
{}

void Particle::update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* particleSystem) {

    sf::Vector2f addVelocity = velocity_ * dt;
    position_ += addVelocity;
    if (lifetime_ == -1)
        lifetime_ -= dt;


    // Example: react to nearby particles
    for (auto* other : nearby) {
        ;
    }
}

void Particle::addVelocity(Particle* other, double attraction)
{
    if (!other || other == this)
        return; // skip invalid or self

    double ox = other->position_.x;
    double oy = other->position_.y;

    double dx = position_.x - ox;
    double dy = position_.y - oy;

    double dist = std::sqrt(dx * dx + dy * dy);

    if (dist != 0.0)
    {
        double div = - dist / attraction;

        velocity_.x += dx / div;
        velocity_.y += dy / div;
    }
}

void Particle::move(Particle* other, double attraction)
{
    sf::Vector2f otherPos = other->getPosition();
    double dx = position_.x - otherPos.x;
    double dy = position_.y - otherPos.y;

    double dist = std::sqrt(dx * dx + dy * dy);

    //printf("what");
    if (!(dist == 0))
    {
    position_.x += ((position_.x - otherPos.x) / ((dist / attraction)));
    position_.y += ((position_.y - otherPos.y) / ((dist / attraction)));
    }
}

/**
 * if attraction negative then it attracts
 */
void Particle::nearAddVelocity(Particle* other, double attraction, double maxDist)
{

    if (!other || other == this)
        return; // skip invalid or self

    double ox = other->position_.x;
    double oy = other->position_.y;

    double dx = position_.x - ox;
    double dy = position_.y - oy;

    double dist = std::sqrt(dx * dx + dy * dy);

    if (dist > maxDist)
        return;

    if (dist != 0.0)
    {
        double div = - dist / attraction;

        velocity_.x += dx / div;
        velocity_.y += dy / div;
    }
}

void Particle::applyForce(const sf::Vector2f& force) {
    // Default: no mass, so treat as acceleration
    velocity_ += force;
}

bool Particle::isAlive() const {
    return lifetime_ > 0.f || lifetime_ == -1;
}

void Particle::draw(sf::RenderWindow& window) const {
    sf::CircleShape shape(radius_);
    shape.setPosition(position_+sf::Vector2f(-radius_, -radius_)); // center the shape on position
    shape.setFillColor(color_);
    window.draw(shape);
}

sf::Vector2f Particle::getPosition() const
{
    return position_;
}

void Particle::setPosition(const sf::Vector2f& pos)
{
    position_ = pos;
}