#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "randomgenerators.hpp"

class ParticleSystem;

class Particle {
public:
    Particle(const sf::Vector2f& position, const sf::Vector2f& velocity, float radius, float lifetime);
    virtual ~Particle() = default; // 👈 must have a virtual destructor

    virtual void update(float dt, const std::vector<Particle*>& nearby, ParticleSystem* system);
    virtual void draw(sf::RenderWindow& window) const;
    void addVelocity(Particle*, double attraction);
    void nearAddVelocity(Particle* other, double attraction, double maxDist);
    void move(Particle* other, double attraction);
    sf::Vector2f position_;
    sf::Vector2f velocity_;


    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);

    virtual bool isAlive();

    float getDetectionRange() const { return detectionRange_; }
    void setDetectionRange(float range) { detectionRange_ = range; }
    void applyForce(const sf::Vector2f& force);
    size_t getHash() const {return hash;};

    bool isTouching(Particle* other);
    float dist(Particle* other);

protected:

    float radius_;
    float lifetime_;
    float detectionRange_ = 64.f; // default, adjustable
    sf::Color color_;
    size_t hash;
};

namespace std{
    template<>
    struct hash<Particle> {
        size_t operator()(const Particle& p) const{
            return p.getHash();
        }
    };
}