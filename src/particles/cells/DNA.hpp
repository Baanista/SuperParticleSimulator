#pragma once
#include <random>
#include <vector>
#include <memory>
#include <cstddef>
#include <utility>
#include "../molecule/molecule.hpp"
// Forward declaration if MoleculeType comes from another header

enum class In_Genes : std::size_t
{
    CONST,
    ATP,
    RADIUS,
    ANGLE,
    LIFETIME,
    AGE,
    COUNT
};

enum class Out_Genes : std::size_t
{
    METABOLIZE_SUGAR,
    PHOTOSYNTHESIZE,
    ROTATE,
    OUTRED,
    OUTBLUE,
    OUTGREEN,
    DUPLICATE,
    COUNT
};

enum class Limiter : std::size_t
{
    NONE,
    GREATER,
    LESS,
    COUNT
};

// 1. Declare DNA as a standalone class to eliminate circular dependency
class DNA
{
public:
    class Connection
    {
    public:
        Connection(DNA *parentDNA, std::size_t inGene, std::size_t outGene, Limiter limiter, float a, float b, float c)
        {
            this->parentDNA = parentDNA;
            this->inGene = std::min({parentDNA->genes.size() - 1, inGene});
            this->outGene = std::min({parentDNA->genes.size() -1, outGene});
            this->limiter = limiter;
            this->a = a;
            this->b = b;
            this->c = c;
        }

        void run()
        {
            if (!parentDNA)
                return;

            float addValue = a * (*parentDNA)[inGene] + b;

            if (limiter == Limiter::GREATER && !(addValue > c))
            {
                return;
            };
            if (limiter == Limiter::LESS && !(addValue < c))
            {
                return;
            };

            (*parentDNA)[outGene] += addValue;
        };

        Connection mutate(float amount, DNA *parentDNA)
        {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::normal_distribution<float> normalDist(0.0f, amount);
            std::normal_distribution<float> probDist(0.0f, 1);


            std::size_t newInGene = inGene;
            std::size_t newOutGene = outGene;
            Limiter newLimiter = limiter;
            float newA = a + normalDist(gen);
            float newB = b + normalDist(gen);
            float newC = c + normalDist(gen);

            if (normalDist(gen) < (amount * 0.3f))
            {
                std::uniform_int_distribution<std::size_t> limiterDist(0, static_cast<std::size_t>(Limiter::COUNT) - 1);
                newLimiter = static_cast<Limiter>(limiterDist(gen));
            };
            std::uniform_int_distribution<std::size_t> randGeneConnectionGen(0, static_cast<std::size_t>(parentDNA->connections_.size()));
            if (probDist(gen) < (amount * 0.5f))
            {
                newInGene = randGeneConnectionGen(gen);
            }
            if (probDist(gen) < (amount * 0.5f))
            {
                newInGene = randGeneConnectionGen(gen);
            }

            return Connection(parentDNA, newInGene, newOutGene, newLimiter, newA, newB, newC);
        }

    private:
        DNA *parentDNA;
        std::size_t inGene;
        std::size_t outGene;
        float a;
        float b;
        float c;
        Limiter limiter;
    };

    DNA(std::size_t extra_molecule_slots)
    {
        genes.resize(total_sizemin);
        // connections_.resize(0);
        for (auto &gene : genes)
        {
            gene = 0.0f;
        }

        // Initialize CONST gene value to 1.0f
        (*this)[In_Genes::CONST] = 1.0f;
        addConnection(In_Genes::CONST, Out_Genes::ROTATE, Limiter::NONE, 0.01f, 0.0f, 0.0f);
        addConnection(In_Genes::CONST, Out_Genes::PHOTOSYNTHESIZE, Limiter::NONE, 1.f, 0.0f, 0.0f);
        addConnection(In_Genes::CONST, Out_Genes::METABOLIZE_SUGAR, Limiter::NONE, 1.f, 0.0f, 0.0f);
        addConnection(In_Genes::ATP, Out_Genes::OUTRED, Limiter::NONE, 1.f, 0.0f, 0.0f);
        addConnection(In_Genes::AGE, Out_Genes::OUTBLUE, Limiter::NONE, 1.f, 0.0f, 0.0f);


        addConnection(In_Genes::RADIUS, Out_Genes::DUPLICATE, Limiter::NONE, 10.0f, -250.f, 0.0f);

        // absorbing
        addConnection(In_Genes::CONST, MoleculeType::CarbonDioxide, Limiter::NONE, 10.f, 0.0f, 0.0f);
        addConnection(In_Genes::CONST, MoleculeType::Water, Limiter::NONE, 10.f, 0.0f, 0.0f);
        addConnection(In_Genes::CONST, MoleculeType::Phospholipid, Limiter::NONE, 10.f, 0.0f, 0.0f);
    }

    DNA()
    {

        genes.resize(total_sizemin);
        // connections_.resize(0);
        for (auto &gene : genes)
        {
            gene = 0.0f;
        }
        
    }

    std::shared_ptr<DNA> mutate(float amount)
    {
        auto outDNA = std::make_shared<DNA>();

        // Copy and mutate existing connections
        outDNA->connections_.reserve(connections_.size());
        for (auto conn : connections_)
        {
            outDNA->connections_.push_back(conn.mutate(amount, outDNA.get()));
        }

        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> probDist(0.0f, 1.0f);

        // adds new gene hidden layer
        if (probDist(gen) < amount * 0.3f)
        {
            outDNA->genes.push_back(0.0f);
        }

        // removes new gene hidden layer
        if (probDist(gen) < amount * 0.1f && outDNA->genes.size() > total_sizemin)
        {
            outDNA->genes.pop_back();
        }

        // Chance to add a new random connection
        if (probDist(gen) < amount * 0.5f)
        {
            std::uniform_int_distribution<std::size_t> randGeneConnectionGen(0, static_cast<std::size_t>(outDNA->genes.size()));

            std::uniform_int_distribution<std::size_t> limiterDist(0, static_cast<std::size_t>(Limiter::COUNT) - 1);
            std::normal_distribution<float> weightDist(0.0f, 1.0f);

            outDNA->addConnection(
                static_cast<size_t>(randGeneConnectionGen(gen)),
                static_cast<size_t>(randGeneConnectionGen(gen)),
                static_cast<Limiter>(limiterDist(gen)),
                weightDist(gen),
                weightDist(gen),
                weightDist(gen));
        }

        if (probDist(gen) < amount * 0.6f)
        {
            std::uniform_int_distribution<std::size_t> randConnectionGen(0, static_cast<std::size_t>(outDNA->connections_.size()));

            outDNA->connections_.push_back(connections_[randConnectionGen(gen)].mutate(amount, outDNA.get()));
        }

        // Chance to remove a random connection if we have redundant ones
        if (!outDNA->connections_.empty() && probDist(gen) < amount * 0.1f)
        {
            std::uniform_int_distribution<std::size_t> removeDist(0, outDNA->connections_.size() - 1);
            outDNA->connections_.erase(outDNA->connections_.begin() + removeDist(gen));
        }

        return outDNA;
    }

    void run()
    {
        (*this)[In_Genes::CONST] = 1.0f;
        for (auto connection : connections_)
        {
            (*this)[In_Genes::CONST] = 1.0f;
            connection.run();
        }
        // (*this)[MoleculeType::Phospholipid] = 14;
    };

    // Indexing Operators
    template <typename T>
    float &operator[](T index)
    {
        return genes[convertToIndex(index)];
    };

    static inline std::size_t convertToIndex(In_Genes index)
    {
        return static_cast<std::size_t>(index);
    };

    static inline std::size_t convertToIndex(Out_Genes index)
    {
        return static_cast<std::size_t>(In_Genes::COUNT) + static_cast<std::size_t>(index);
    };

    // for cell mol absorbing or droping
    static inline std::size_t convertToIndex(MoleculeType index)
    {
        return static_cast<std::size_t>(In_Genes::COUNT) + static_cast<std::size_t>(Out_Genes::COUNT) + static_cast<std::size_t>(index);
    };

    static inline std::size_t convertToIndex(std::size_t index)
    {
        return index;
    };

    // for the input for cytoplasm
    static inline std::size_t convertCytoplasmToIndex(MoleculeType type)
    {
        const std::size_t shiftAmount = static_cast<std::size_t>(In_Genes::COUNT) + static_cast<std::size_t>(Out_Genes::COUNT) + static_cast<std::size_t>(MoleculeType::COUNT);
        return shiftAmount + static_cast<std::size_t>(type);
    };

    static inline std::size_t convertCytoplasmToIndex(std::size_t type)
    {
        return convertCytoplasmToIndex(static_cast<std::size_t>(type));
    };
    // for the input for cytoplasm
    template <typename T>
    float &getCytoplasmGene(T type)
    {

        return genes[convertCytoplasmToIndex(type)];
    }

    // for the input for cytoplasm
    template <typename T>
    void setCytoplasmValue(T type, float value)
    {
        getCytoplasmGene(type) = value; // Fixed:Direct float assignment
    };

    template <typename T>
    void setValue(T type, float val)
    {
        (*this)[type] = val; // Fixed: Direct float assignment
    };

    template <typename T>
    float resetValue(T type)
    {
        float out = (*this)[type]; // Fixed: Direct float value read
        (*this)[type] = 0.0f;
        return out;
    };

    template <typename T>
    float getGene(T type)
    {
        return (*this)[type];
    };

    // Your template missing default c = 0.0f parameter
    template <typename T, typename U>
    void addConnection(T first, U other, Limiter limiter = Limiter::NONE, float a = 1.0f, float b = 0.0f, float c = 0.0f)
    {
        std::size_t first_connection = convertToIndex(first);
        std::size_t other_connection = convertToIndex(other);
        connections_.emplace_back(
            this,
            first_connection,
            other_connection,
            limiter,
            a,
            b,
            c);
    }

private:
    std::vector<Connection> connections_;
    std::vector<float> genes;
    static inline std::size_t total_sizemin =
        static_cast<std::size_t>(In_Genes::COUNT) +
        static_cast<std::size_t>(Out_Genes::COUNT) +
        static_cast<std::size_t>(MoleculeType::COUNT) * 2;
};