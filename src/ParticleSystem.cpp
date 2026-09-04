#include "ParticleSystem.hpp"
#include <cstdlib>
#include <cmath>

ParticleSystem::ParticleSystem(unsigned int maxParticles, sf::Vector2f size, BorderBehavior behavior)
    : maxParticles_(maxParticles),
      gravityField_(std::make_unique<GravityField>(size.x, size.y, 0.5f, 100.0f)),
      borderBehavior(behavior) {
    
    this->size = size;
    
    particles_ = std::vector<std::shared_ptr<Particle>>();
}

void ParticleSystem::emit(const sf::Vector2f& position, unsigned int count) {
    for (unsigned int i = 0; i < count && particles_.size() < maxParticles_; ++i) {
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.f * 3.14159f;
        float speed = static_cast<float>(rand()) / RAND_MAX * 100.f + 50.f;
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        float radius = static_cast<float>(rand()) / RAND_MAX * 3.f + 1.f;
        float lifetime = static_cast<float>(rand()) / RAND_MAX * 2.f + 1.f;


        emitOne<Particle>(position, velocity, radius, lifetime);
    }
}

void ParticleSystem::addParticle(std::shared_ptr<Particle> particle) {
    particles_.push_back(particle);
}

void ParticleSystem::update(float dt) {
    grid_.clear();


    for (auto& p : particles_) {
        grid_.insert(p.get(), p->getPosition());
    }

    std::vector<ParticleMatter*> matterParticles;
    matterParticles.reserve(particles_.size());
    for (auto& p : particles_) {
        if (auto* matter = dynamic_cast<ParticleMatter*>(p.get())) {
            matterParticles.push_back(matter);
        }
    }

    // 3️⃣ Build gravity field and apply forces
    if (nBodyGravity) {
        if (!matterParticles.empty()) {
            gravityField_->build(matterParticles);
        }
    }


    const sf::Vector2f gravityCenter(0.f, 0.f);
    const float G = 10000.f;

    // 2️⃣ Update all particles
    for (size_t i = 0; i < particles_.size(); ) {
        auto& particle = particles_[i];

        particle->applyForce({0.f, downwardGravity * dt});
        // make particles wrap around the bounds of the simulation area
        sf::Vector2f pos = particle->getPosition();
        if (particle->getPosition().x < 0) 
        {
            if (borderBehavior == WRAP)
                pos.x += size.x;
            else if (borderBehavior == BOUNCE)
                particle->velocity_.x *= -1;
        }
        else if (pos.x > size.x) 
        {
            if (borderBehavior == WRAP)
                pos.x -= size.x;
            else if (borderBehavior == BOUNCE)
                particle->velocity_.x *= -1;
        }
        if (pos.y < 0) 
        {
            if (borderBehavior == WRAP)
                pos.y += size.y;
            else if (borderBehavior == BOUNCE)
                particle->velocity_.y *= -1;
        }
        else if (pos.y > size.y) 
        {
            if (borderBehavior == WRAP)
                pos.y -= size.y;
            else if (borderBehavior == BOUNCE)
                particle->velocity_.y *= -1;
        }
        particle->setPosition(pos);

        float detectionRange = particle->getDetectionRange(); // 👈 new per-particle value
        auto neighbors = grid_.queryRange(particle->getPosition(), detectionRange);
        if (nBodyGravity){
            
            
            if (auto* matter = dynamic_cast<ParticleMatter*>(particle.get())) {
                sf::Vector2f gravityForce = gravityField_->computeForceOn(*matter);
                matter->applyForce(gravityForce * dt);
            }
        }

        particle->update(dt, neighbors, this);

        ++i;
    }

    std::erase_if(particles_, [](const std::shared_ptr<Particle>& p) {
        return !p->isAlive();
    });

    if (!pendingParticles_.empty()) {
        particles_.insert(
            particles_.end(),
            std::make_move_iterator(pendingParticles_.begin()),
            std::make_move_iterator(pendingParticles_.end())
        );
        pendingParticles_.clear();
    }

    std::cout << "Particle Amount: " << particles_.size() << std::endl; 
}


void ParticleSystem::draw(sf::RenderWindow& window) {
    for (const auto& p : particles_)
        p->draw(window);
}
