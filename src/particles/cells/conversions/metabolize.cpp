#include "../cell.hpp"

float Cell::metabolize_sugar(float amount) {
    if (amount <= 0.0f) return 0.0f;

    // Get available resources
    float availableSugar = cytoplasm_[MoleculeType::Sugar];
    float availableOxygen = cytoplasm_[MoleculeType::Oxygen];

    // Reaction is bottlenecked by requested amount, available sugar, and available oxygen (1:1 ratio)
    float printableAmount = std::min({amount, availableSugar, availableOxygen});

    if (printableAmount <= 0.0f) {
        return amount; // 0 mass converted, all requested amount remaining
    }

    // 1. Consume reactants (1 part Sugar, 1 part Oxygen)
    cytoplasm_[MoleculeType::Sugar] -= printableAmount;
    cytoplasm_[MoleculeType::Oxygen] -= printableAmount;

    // 2. Produce energy (10 * amount ATP)
    atp_ += 10.0f * printableAmount;

    // 3. Produce byproducts (1 part CO2, 1 part Water)
    cytoplasm_[MoleculeType::CarbonDioxide] += printableAmount;
    cytoplasm_[MoleculeType::Water] += printableAmount;

    // Return the unreacted portion of the requested amount
    return amount - printableAmount;
}

float Cell::photosynthesize(float amount) {
    if (amount <= 0.0f) return 0.0f;

    // 1 part CO2 + 1 part Water -> 1 part Sugar + 1 part Oxygen
    float availableCO2 = cytoplasm_[MoleculeType::CarbonDioxide];
    float availableWater = cytoplasm_[MoleculeType::Water];

    // Bottleneck reaction by requested amount and available reactants
    float printableAmount = std::min({amount, availableCO2, availableWater});

    if (printableAmount <= 0.0f) {
        return amount; // Cannot run reaction, return full unreacted amount
    }

    // 1. Consume Reactants & Energy
    cytoplasm_[MoleculeType::CarbonDioxide] -= printableAmount;
    cytoplasm_[MoleculeType::Water] -= printableAmount;

    // 2. Produce Products (1:1 mass balance)
    cytoplasm_[MoleculeType::Sugar] += printableAmount;
    cytoplasm_[MoleculeType::Oxygen] += printableAmount;

    // Return unreacted amount
    return amount - printableAmount;
}