#pragma once
#include <SFML/Graphics.hpp>

class Particle {
public:
    Particle(const sf::Vector2f& position, const sf::Vector2f& velocity, float radius, float lifetime);
    virtual ~Particle() = default; // 👈 must have a virtual destructor

    virtual void update(float dt, const std::vector<Particle*>& nearby);
    virtual void draw(sf::RenderWindow& window) const;
    void addVelocity(Particle*, double attraction);
    void nearAddVelocity(Particle* other, double attraction, double maxDist);
    void move(Particle* other, double attraction);
    sf::Vector2f position_;
    sf::Vector2f velocity_;


    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);

    bool isAlive() const;

    float getDetectionRange() const { return detectionRange_; }
    void setDetectionRange(float range) { detectionRange_ = range; }
    void applyForce(const sf::Vector2f& force);
    

    protected:

        float radius_;
        float lifetime_;
        float detectionRange_ = 64.f; // default, adjustable
        sf::Color color_;
};
