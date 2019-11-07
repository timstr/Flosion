#include <Flosion/Objects/Resampler.hpp>

namespace flo {

    void ResamplerState::reset() noexcept {
        buffer.silence();
        posCoarse = 0;
        posFine = 0.0;
    }

    Resampler::Resampler()
        : input(this)
        , timeSpeed(this, 1.0) {
    }

    void Resampler::renderNextChunk(flo::SoundChunk& chunk, ResamplerState* state){
        // TODO: different types of interpolation and smoothing and junk

        const auto refillBuffer = [&]() -> void {
            input.getNextChunkFor(state->buffer, this, state);
            assert(state->posCoarse == flo::SoundChunk::size);
            state->posCoarse = 0;
        };

        const auto getNextSample = [&](double speed) -> flo::Sample {
            auto acc = flo::Sample{};
            auto count = std::uint8_t{0};

            state->posFine += speed;

            if (state->posFine < 1.0){
                return flo::Sample{state->buffer[state->posCoarse]};
            }

            while (state->posFine >= 1.0){
                state->posFine -= 1.0;
                acc += state->buffer[state->posCoarse];
                ++count;
                ++state->posCoarse;
                if (state->posCoarse == flo::SoundChunk::size){
                    refillBuffer();
                }
            }

            return acc / static_cast<float>(count);
        };

        for (std::uint16_t i = 0; i < flo::SoundChunk::size; ++i){
            state->adjustTime(i);
            state->speed = timeSpeed.getValue(state);
            chunk[i] = getNextSample(state->speed);
        }
    }

    double Resampler::getTimeSpeed(const flo::SoundState* context) const noexcept {
        // NOTE: the speed is cached in the state to prevent an infinite loop
        return findOwnState(context)->speed;
    }

} // namespace flo
