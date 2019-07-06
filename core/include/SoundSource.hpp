#pragma once

#include <Immovable.hpp>
#include <SoundChunk.hpp>
#include <StateOwner.hpp>
#include <SoundState.hpp>

#include <vector>

namespace flo::sound {

    class SoundInput;

    class SoundSource : private Immovable {
    public:

        /*
         * Produces the next chunk of sound. The SoundSource's state as found
         * in the given context may be updated.
         */
        virtual void getNextChunk(SoundChunk& chunk, const SoundState* context) = 0;

        /*
         * Retrieves the current (relative to the given context) speed of time.
         * For realtime SoundSources, this will always be 1.
         */
        virtual double getTimeSpeed(const SoundState* context) const noexcept = 0;

        /*
         * Returns whether or not the SoundSource owns no state, that is, it
         * has no data to store between calls to getNextChunk().
         */
        virtual bool isStateless() const noexcept = 0;

        /*
         * Returns whether or not the SoundSource is realtime, that is, it draws
         * upon each possible input exactly once per chunk. For SingleInputs, this
         * means a single call, and for MultiInputs, this means a single call for each
         * key.
         */
        virtual bool isRealTime() const noexcept = 0;

        /*
         * Returns whether or not the SoundSource is uncontrolled, that is, it
         * owns state, but that state is outside of the control of this program,
         * and its sound data typically is available only in real time.
         * Such a SoundSource may only be drawn from with an input where the
         * multiplicity to this source is 1 and the overal network to this
         * source is realtime. Examples include microphone input, MIDI keyboard
         * input, sound data coming from the internet, etc
         * See class SoundInput.
         */
        virtual bool isUncontrolled() const noexcept = 0;

        // TODO

    private:
        std::vector<SoundInput*> m_inputs;
        std::vector<const SoundInput*> m_dependents;
    };

    class StatefulSoundSource : virtual public SoundSource, virtual public StateOwner {
        // TODO
    };

    class RealTimeSoundSource : virtual public SoundSource {
    public:
        // TODO

    private:
        double getTimeSpeed(const SoundState* context) const noexcept override;
    };

    class StatelessSoundSource : public RealTimeSoundSource {
        // TODO
    };

} // namespace flo::sound
