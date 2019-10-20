#include <Flosion/Objects/FunctionsBase.hpp>

namespace flo {
    
    UnaryFunction::UnaryFunction(double defaultValue)
        : input(this, defaultValue) {
    }

    BinaryFunction::BinaryFunction(double defaultValue1, double defaultValue2)
        : input1(this, defaultValue1)
        , input2(this, defaultValue2) {

    }

} // namespace flo
