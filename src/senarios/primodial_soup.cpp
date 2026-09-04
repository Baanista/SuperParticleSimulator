#include "primodial_soup.hpp"

void primordial_soup(
    ParticleSystem* system, 
    const std::array<std::size_t, MoleculeType::COUNT>& counts
) {
    if (!system) return;

    const sf::Vector2f minBounds(0.0f, 0.0f);
    const sf::Vector2f maxBounds = system->size;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posX(minBounds.x, maxBounds.x);
    std::uniform_real_distribution<float> posY(minBounds.y, maxBounds.y);
    std::uniform_real_distribution<float> vel(-15.0f, 15.0f);

    for (std::size_t i = 0; i < MoleculeType::COUNT; ++i) {
        auto type = static_cast<MoleculeType>(i);
        std::size_t particleCount = counts[i]; // Works directly with std::array

        for (std::size_t n = 0; n < particleCount; ++n) {
            sf::Vector2f randomPos(posX(gen), posY(gen));
            sf::Vector2f randomVel(vel(gen), vel(gen));

            system->emitOne<Molecule>(
                randomPos,
                randomVel,
                type
            );
        }
    }
}