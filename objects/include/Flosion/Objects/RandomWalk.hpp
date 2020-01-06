#pragma once

#include <Flosion/Core/BorrowingNumberSource.hpp>
#include <random>

namespace flo {

    class RandomWalkState : public flo::State {
    public:

        void reset() noexcept override;

        double value;
        double velocity;
    };

    class RandomWalk : public flo::BorrowingNumberSourceTemplate<RandomWalkState> {
    public:
        RandomWalk();

        flo::NumberSourceInput speed;
        flo::NumberSourceInput damping;
        flo::NumberSourceInput bias;

    private:
        std::uniform_real_distribution<double> m_dist;

        double evaluate(RandomWalkState* state, const flo::SoundState* context) const noexcept override;
    };

} // namespace flo
