#include "../cell.hpp"
#include "../../../ParticleSystem.hpp"

float Cell::convert_molecule(Cytoplasm in, Cytoplasm out, float amount, float atp_change)
{
    if (amount <= 0.0f) return 0.0f;

    //to get actual usable amount that can be used
    float printableAmount = amount;
    for (size_t type = 0; type < MoleculeType::COUNT; type++)
    {
        if (in.cytoplasm_[type] != 0)
        {
            printableAmount = std::min({printableAmount, cytoplasm_[type] / in.cytoplasm_[type]});
        }
    };



    if (atp_change < 0.0f)
    {
        float atp_needed = -atp_change * printableAmount;
        if (atp_ < atp_needed)
        {
            // Scale printableAmount down to the max ATP available
            printableAmount = atp_ / -atp_change;

        }
    }


    atp_ += atp_change * printableAmount;

    if (printableAmount < 0) return amount;

    for (size_t type = 0; type < MoleculeType::COUNT; type++)
    {
        cytoplasm_[type] -= in.cytoplasm_[type] * printableAmount;
        cytoplasm_[type] += out.cytoplasm_[type] * printableAmount;
    }

    return amount - printableAmount;
};

float Cell::metabolize_sugar(float amount, float dt) {
    if (amount <= 0.0f) return 0.0f;
    float metabolizeLimiter = radius_ * .1;
    float metabolizeAmount = std::min(metabolizeLimiter , amount) * dt;

    return convert_molecule(
        Cytoplasm().add(MoleculeType::Sugar).add(MoleculeType::Oxygen, 2), 
        Cytoplasm().add(MoleculeType::CarbonDioxide, 2).add(MoleculeType::Water),
        metabolizeAmount,
        20.0f
    );
}

float Cell::photosynthesize(ParticleSystem* system, float amount, float dt) {
    if (amount <= 0.0f) return 0.0f;
    float photoAmout = position_.y / system->size.y;
    float actual_amount = std::min(photoAmout, amount) * dt;



    return amount - convert_molecule(
        Cytoplasm().add(MoleculeType::Water).add(MoleculeType::CarbonDioxide, 2),
        Cytoplasm().add(MoleculeType::Sugar).add(MoleculeType::Oxygen, 2),
        actual_amount,
        0
    );
}