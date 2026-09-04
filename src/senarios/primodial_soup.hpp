#pragma once
#include <array>
#include <random>
#include "../ParticleSystem.hpp"
#include "../particles/molecule/molecule.hpp"

// Declaration using std::array
void primordial_soup(
    ParticleSystem* system, 
    const std::array<std::size_t, MoleculeType::COUNT>& counts
);