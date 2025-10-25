#include <SFML/Graphics.hpp>
#include <iostream>
#include "ParticleSystem.hpp"
#include "particle_types.hpp"

void spawnOrbitalRing(ParticleSystem& system, int count, float radius, float speed, sf::Vector2f center, float mass = 1.f, float G = 1000.0f)
{
    // Creates 'count' particles orbiting around (0, 0)
    for (int i = 0; i < count; ++i)
    {
        float angle = static_cast<float>(rand()) / RAND_MAX * 2.f * 3.1415926f;

        // Random distance from center [0, radius] — uniform area distribution
        float r = std::sqrt(static_cast<float>(rand()) / RAND_MAX) * radius;

        // Position (centered around origin)
        sf::Vector2f pos(r * std::cos(angle), r * std::sin(angle));
        pos += center;

        // Tangential direction (90° rotated from radius vector)
        sf::Vector2f tangent(-std::sin(angle), std::cos(angle));

        // Circular orbital speed (approximate)
        float speed = std::sqrt(G * mass / (r + 1e-3f)); // prevent divide-by-zero

        // Final velocity vector
        sf::Vector2f vel = tangent * speed;

        // Spawn orbiting particle
        system.addParticle(std::make_unique<ParticleMatter>(pos, vel, 5.f, mass));
;
    }
}

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "Particle Simulation");
    window.setFramerateLimit(144);

    ParticleSystem particles(200000);

    spawnOrbitalRing(particles, 800, 350.f, 100.f, {960.0f, 540.0f}, 1.f); 

    sf::Clock clock;

    sf::View camera(sf::Rect<float>({0.f, 0.f}, {1920u, 1080u}));
    float cameraSpeed = 500.f;  // movement speed
    float zoomLevel = 1.f;      // zoom factor

    std::cout << "Particle System Initialized" << std::endl;

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > .2)
        {
            dt = .2;
        }
        std::cout << dt << std::endl;
        // --- Event loop (SFML 3.0 style) ---
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

                sf::Vector2f moveDir(0.f, 0.f);
                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    switch (keyPressed->scancode) {
                        case sf::Keyboard::Scancode::A:
                            moveDir.x -= 1.f;
                            break;
                        case sf::Keyboard::Scancode::D:
                            moveDir.x += 1.f;
                            break;
                        case sf::Keyboard::Scancode::W:
                            moveDir.y -= 1.f;

                            break;
                        case sf::Keyboard::Scancode::S:
                            moveDir.y += 1.f;

                            break;
                    }
                }

                if (moveDir.x != 0.f || moveDir.y != 0.f)
                    camera.move(moveDir * cameraSpeed * dt);
        }


        // Emit particles at mouse position if left mouse button is held
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

            float angle = static_cast<float>(rand()) / RAND_MAX * 2.f * 3.1415926f;
            float speed = 50.f + static_cast<float>(rand()) / RAND_MAX * 150.f; // range 50–200
            // sf::Vector2f velocity(-std::sin(angle) * speed, std::cos(angle) * speed);
            sf::Vector2f velocity(0, 0);

            particles.addParticle(std::make_unique<ParticleMatter>(
                sf::Vector2f(static_cast<float>(worldPos.x), static_cast<float>(worldPos.y)),
                velocity, 3.f, 1.f));
        }
        
        

        // Update and draw
        particles.update(dt);

        window.clear(sf::Color::Black);
        window.setView(camera);
        particles.draw(window);
        window.display();
    }
}
