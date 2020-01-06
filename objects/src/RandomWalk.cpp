#include <Flosion/Objects/RandomWalk.hpp>

#include <Flosion/Util/RNG.hpp>

namespace flo {



    void RandomWalkState::reset() noexcept {
        value = 0.0;
        velocity = 0.0;
    }

    RandomWalk::RandomWalk()
        : speed(this, 0.001)
        , damping(this, 0.5)
        , bias(this, 0.1)
        , m_dist(-1.0, 1.0) {

    }

    double RandomWalk::evaluate(RandomWalkState* state, const flo::SoundState* context) const noexcept {
        const auto sp = speed.getValue(context);
        const auto d = damping.getValue(context);
        const auto b = bias.getValue(context);
        auto deltaVelocity = (m_dist(util::getRandomEngine()) - state->value * b) * sp;
        state->velocity = (state->velocity * d) + deltaVelocity;
        state->value += state->velocity;
        return state->value;
    }

} // namespace flo
