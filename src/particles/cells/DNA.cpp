#include "cell.hpp"
#include <vector>


// this class works kinda like a nural network. Receving inputs and giving outputs
class Cell::DNA
{
private:
    DNA()
    {

    };
    // std::array<float, >

    class Gene{
        public:
            float value;
            

        private:
            class Connection{
                // var connection pointer or int
                float a;
                float b;
            };
    };
public:

    DNA& set_IN_Gene();

    enum In_Genes : size_t{
        ATP,
        RADIUS,
        COUNT
    };

    enum Out_Genes : size_t{
        METABOLIZE_SUGAR,
        PHOTOSYNTHESIZE
    };
    //
};