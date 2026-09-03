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
        system.addParticle(std::make_shared<ParticleMatter>(pos, vel, 5.0f, mass));
;
    }
}

int main()
{
    sf::RenderWindow window = sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "Particle Simulation");
    window.setFramerateLimit(144);

    ParticleSystem particles(200000, {1920.f, 1080.f}, ParticleSystem::BOUNCE);
    particles.nBodyGravity = false;

    //spawnOrbitalRing(particles, 3000, 540.f, 0, {960.0f, 540.0f}, 1.0f); 

    sf::Clock clock;

    sf::View camera(sf::Rect<float>({0.f, 0.f}, {1920u, 1080u}));
    float cameraSpeed = 500.f;  // movement speed
    float zoomLevel = 1.f;      // zoom factor

    std::cout << "Particle System Initialized" << std::endl;

    sf::Vector2f velocity(0, 0);

    for (float x = 0; x < particles.size.x; x += particles.size.x / 100) {
        for (float y = 0; y < particles.size.y; y += particles.size.y / 25) {

            particles.addParticle(std::make_shared<Molecule>(
                sf::Vector2f(x, y),          // Explicit position
                sf::Vector2f(0.f, 0.f),
                MoleculeProperties::Water,
                3
            ));
        }
    }
    // particles.addParticle(std::make_shared<ParticleMatter>(
    // sf::Vector2f(static_cast<float>(100), static_cast<float>(100)),
    // sf::Vector2f{0.f, 0.f}, 3.f, 1.f)
    // );

    // particles.addParticle(std::make_shared<Cell>(
    //     sf::Vector2f(200.0f, 200.0f),
    //     sf::Vector2f{0.f, 0.f},
    //     20.0f
    // ));

    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        if (dt > .2)
        {
            dt = .2;
        }
        std::cout << "dt: " << dt << std::endl;
        // --- Event loop (SFML 3.0 style) ---
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            bool moveLeft = false, moveRight = false, moveUp = false, moveDown = false;
            
            sf::Vector2f moveDir(0.f, 0.f);
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                switch (keyPressed->scancode) {
                    case sf::Keyboard::Scancode::A:      moveLeft = true;  break;
                    case sf::Keyboard::Scancode::D:      moveRight = true; break;
                    case sf::Keyboard::Scancode::W:      moveUp = true;    break;
                    case sf::Keyboard::Scancode::S:      moveDown = true;  break;

                }
            }

            if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                switch (keyReleased->scancode) {
                    case sf::Keyboard::Scancode::A:      moveLeft = false;  break;
                    case sf::Keyboard::Scancode::D:      moveRight = false; break;
                    case sf::Keyboard::Scancode::W:      moveUp = false;    break;
                    case sf::Keyboard::Scancode::S:      moveDown = false;  break;
                    default: break;
                }
            }
            const float panFactor = 1.0f;
            float panSpeed = camera.getSize().x * panFactor * dt; 

            if (moveLeft)  camera.move({-panSpeed, 0});
            if (moveRight) camera.move({panSpeed, 0});
            if (moveUp)    camera.move({0, -panSpeed});
            if (moveDown)  camera.move({0, panSpeed});

            if (const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                float zoomAmount = (mouseWheel->delta > 0) ? 0.8f : 1.05f;
                camera.zoom(zoomAmount);
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
            sf::Vector2f velocity(0.f, 0.f);

            // particles.addParticle(std::make_unique<ParticleMatter>(
            //     sf::Vector2f(static_cast<float>(worldPos.x), static_cast<float>(worldPos.y)),
            //     velocity, 5.f, 10.f));

            // particles.addParticle(std::make_unique<Particle>(
            //     sf::Vector2f(static_cast<float>(worldPos.x), static_cast<float>(worldPos.y)),
            //     velocity, 5.f, 10.f
            // ));
        }
        
        

        // Update and draw
        particles.update(dt); // convert to milliseconds

        window.clear(sf::Color::Black);
        window.setView(camera);
        particles.draw(window);
        window.display();
    }
}
