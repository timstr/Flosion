#pragma once

#include <Flosion/Core/Sample.hpp>
#include <Flosion/Core/SingleSoundInput.hpp>

#include <vector>

namespace flo {

    // SoundQueue is a circular queue of audio samples, intended to act as a
    // delay line for any audio processing that requires more than a Chunk
    class SoundQueue {
    public:
        // The size must be larger than the Chunk size.
        SoundQueue(std::size_t nSamples);

        // Set the size of the queue. The size must be larger than the Chunk size.
        // This will clear the queue and fill it with silence.
        void resize(std::size_t nSamples);

        std::size_t size() const noexcept;

        void clear();

        // Advance the queue by a number of samples, while feeding in silence
        void advance(std::size_t nSamples);

        // Advance the queue by a number of samples, while feeding in audio from the given input
        void advance(std::size_t nSamples, SingleSoundInput& input, const SoundNode* target, const SoundState* state);

        Sample& operator[](std::size_t index) noexcept;

    private:

        std::vector<Sample> m_data;
        std::size_t m_index;
        std::size_t m_front;
    };

} // namespace flo
