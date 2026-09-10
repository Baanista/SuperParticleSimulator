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
    COUNT
};

enum class Out_Genes : std::size_t
{
    METABOLIZE_SUGAR,
    PHOTOSYNTHESIZE,
    ROTATE,
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
        Connection(DNA* parentDNA, std::size_t inGene, std::size_t outGene, Limiter limiter, float a, float b, float c)
            : parentDNA(parentDNA), inGene(inGene), outGene(outGene), limiter(limiter), a(a), b(b), c(c) {}

        void run()
        {
            if (!parentDNA) return;

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

        Connection mutate(float amount, DNA* parentDNA){
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::normal_distribution<float> normalDist(0.0f, amount);

            std::size_t newInGene = inGene;
            std::size_t newOutGene = outGene;
            Limiter newLimiter = limiter;
            float newA = a + normalDist(gen);
            float newB = b + normalDist(gen);
            float newC = c + normalDist(gen);

            if (normalDist(gen) < (amount * 0.1f)){
                std::uniform_int_distribution<std::size_t> limiterDist(0, static_cast<std::size_t>(Limiter::COUNT) - 1);
                newLimiter = static_cast<Limiter>(limiterDist(gen));
            };
            std::uniform_int_distribution<std::size_t> randGeneConnectionGen(0, static_cast<std::size_t>(parentDNA->connections_.size()));
            if (normalDist(gen) < (amount * 0.5f)){
                newInGene = randGeneConnectionGen(gen);
            }
            if (normalDist(gen) < (amount * 0.5f)){
                newInGene = randGeneConnectionGen(gen);
            }

            return Connection(parentDNA, newInGene, newOutGene, newLimiter, newA, newB, newC);
        }

    private:
        DNA* parentDNA;
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
        addConnection(In_Genes::CONST, Out_Genes::ROTATE, Limiter::NONE, 0.001f, 0.0f, 0.0f);
        addConnection(In_Genes::CONST, Out_Genes::PHOTOSYNTHESIZE, Limiter::NONE, 0.001f, 0.0f, 0.0f);
        addConnection(In_Genes::CONST, Out_Genes::METABOLIZE_SUGAR, Limiter::NONE, 0.001f, 0.0f, 0.0f);

    }

    DNA(){
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

        // Chance to add a new random connection
        if (probDist(gen) < amount * 0.3f)
        {
            std::uniform_int_distribution<std::size_t> inDist(0, static_cast<std::size_t>(In_Genes::COUNT) - 1);
            std::uniform_int_distribution<std::size_t> outDist(0, static_cast<std::size_t>(Out_Genes::COUNT) - 1);
            std::uniform_int_distribution<std::size_t> limiterDist(0, static_cast<std::size_t>(Limiter::COUNT) - 1);
            std::normal_distribution<float> weightDist(0.0f, 1.0f);

            outDNA->addConnection(
                static_cast<In_Genes>(inDist(gen)),
                static_cast<Out_Genes>(outDist(gen)),
                static_cast<Limiter>(limiterDist(gen)),
                weightDist(gen),
                weightDist(gen),
                weightDist(gen)
            );
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
        for (auto connections : connections_)
        {
            connections.run();
        }
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

    template <typename T>
    static inline std::size_t convertToIndex(T index)
    {
        return static_cast<std::size_t>(index);
    }

    template <typename T>
    float& getCytoplasmGene(T type) {
        const std::size_t shiftAmount = static_cast<std::size_t>(In_Genes::COUNT) + static_cast<std::size_t>(Out_Genes::COUNT) + static_cast<std::size_t>(MoleculeType::COUNT);
        return genes[shiftAmount + static_cast<std::size_t>(type)];
    }

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
        connections_.emplace_back(
            this, 
            static_cast<std::size_t>(convertToIndex(first)), 
            static_cast<std::size_t>(convertToIndex(other)), 
            limiter, 
            a, 
            b, 
            c
        ); 
    }

private:
    std::vector<Connection> connections_;
    std::vector<float> genes;
    static inline std::size_t total_sizemin = 
        static_cast<std::size_t>(In_Genes::COUNT) +
        static_cast<std::size_t>(Out_Genes::COUNT) +
        static_cast<std::size_t>(MoleculeType::COUNT) * 2;
};