#include <Flosion/Objects/Lowpass.hpp>

namespace flo {

    void LowpassState::reset() noexcept {
        value = {0.0, 0.0};
    }

    Lowpass::Lowpass()
        : input(this)
        , cutoff(this, 0.5) {

    }

    void Lowpass::renderNextChunk(SoundChunk& chunk, LowpassState* state){
        input.getNextChunkFor(chunk, this, state);
        const auto dt = 1.0f / static_cast<float>(sampleFrequency);
        for (int i = 0; i < chunk.size; ++i){
            state->adjustTime(i);
            const auto fc = static_cast<float>(cutoff.getValue(state));
            const auto rc = 1.0f / (2.0f * 3.141592654f * fc);
            const auto a = std::clamp(dt / (rc + dt), 0.0f, 1.0f);

            chunk[i] = state->value + (chunk[i] - state->value) * a;
            state->value = chunk[i];
        }
    }

} // namespace flo
