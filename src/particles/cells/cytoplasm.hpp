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
        space = 0.0f;
    }

    Cytoplasm& add(MoleculeType type, float amount) {
        cytoplasm_[type] += amount;
        space += amount * MoleculeProperties(type).density;
        return *this;
    }

    Cytoplasm& add(MoleculeType type)
    {
        return add(type, 1.0f);
    }

    Cytoplasm& remove(MoleculeType type, float amount)
    {
        cytoplasm_[type] -= amount;
        space -= amount * MoleculeProperties(type).density;
        return *this;
    };

    Cytoplasm& remove(MoleculeType type)
    {
        return remove(type, 1.0f);
    };

    float& operator[](std::size_t index)
    {
        return cytoplasm_[index];
    }

    std::array<float, static_cast<std::size_t>(MoleculeType::COUNT)> cytoplasm_;
    float space;
    
};