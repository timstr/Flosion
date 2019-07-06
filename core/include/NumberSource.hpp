#pragma once

#include <Immovable.hpp>

namespace flo::sound {

    class SoundState;

} // namespace flo::sound

namespace flo::number {

    class NumberSource : private Immovable {
    public:

        using EvaluationFunction = double(*)(const NumberSource*, const sound::SoundState*) noexcept;

        NumberSource(EvaluationFunction) noexcept;

        void evaluate(const sound::SoundState* context) const noexcept;

    private:
        EvaluationFunction m_evalFn;
    };

} // namespace flo::number
