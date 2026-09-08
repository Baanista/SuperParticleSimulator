#pragma once
#include "../molecule/molecule.hpp"
#include <array>
#include <cstddef>
#include <cmath>
#include <memory>

class Cytoplasm
{
public:
    Cytoplasm()
    {
        cytoplasm_.fill(0.0f);
    }

    Cytoplasm& add(MoleculeType type, float amount) {
        cytoplasm_[type] += amount;
        return *this;
    }

    Cytoplasm& add(MoleculeType type)
    {
        cytoplasm_[type] += 1.0f;
        return *this;
    }

    Cytoplasm& remove(MoleculeType type, float amount)
    {
        cytoplasm_[type] -= amount;
        return *this;
    };

    Cytoplasm& remove(MoleculeType type)
    {
        cytoplasm_[type] -= 1.0f;
        return *this;
    };

    float& operator[](std::size_t index)
    {
        return cytoplasm_[index];
    }

    std::array<float, static_cast<std::size_t>(MoleculeType::COUNT)> cytoplasm_;
    
};