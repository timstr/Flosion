#include <Flosion/Objects/Variable.hpp>

namespace flo {

    void VariableState::reset() noexcept {
        value = 0.0;
    }



    Variable::Variable()
        : input(this) {

    }

    double Variable::evaluate(VariableState* state, const flo::SoundState* context) const noexcept {
        const auto v = state->value;
        state->value = input.getValue(context);
        return v;
    }

} // namespace flo
