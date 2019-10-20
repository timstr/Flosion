#include <Flosion/Objects/Functions.hpp>

namespace flo {

    Add::Add() : inputA(this, 1.0), inputB(this, 1.0) {
        
    }
    
    double Add::evaluate(const flo::SoundState* context) const noexcept {
        return inputA.getValue(context) + inputB.getValue(context);
    }

    Multiply::Multiply() : inputA(this, 1.0), inputB(this, 1.0) {
        
    }

    double Multiply::evaluate(const flo::SoundState* context) const noexcept {
        return inputA.getValue(context) * inputB.getValue(context);
    }

} // namespace flo
