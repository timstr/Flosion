#include <Flosion/Objects/Functions.hpp>

namespace flo {
    
    double Add::evaluate(const flo::SoundState* context) const noexcept {
        return input1.getValue(context) + input2.getValue(context);
    }

    Multiply::Multiply() : BinaryFunction(1.0, 1.0) {
        
    }

    double Multiply::evaluate(const flo::SoundState* context) const noexcept {
        return input1.getValue(context) * input2.getValue(context);
    }

    double Subtract::evaluate(const flo::SoundState* context) const noexcept {
        return input1.getValue(context) - input2.getValue(context);
    }

    Divide::Divide() : BinaryFunction(1.0, 1.0) {

    }

    double Divide::evaluate(const flo::SoundState* context) const noexcept {
        return input1.getValue(context) / input2.getValue(context);
    }

} // namespace flo

