#pragma once

#include <Immovable.hpp>

namespace flo::sound {

    // TODO: add a onPlay()/onPause()/onStop() interface to SoundSource
    // These are called recursively whenever a SoundResult starts/pauses/stops
    // Why:
    // - Uncontrolled SoundSources can begin/end whatever they need to do,
    //   e.g. set up a microphone connection
    // But what if the SoundResult is not realtime, i.e. it is rendering offline
    // to a file? Probably can't use uncontrolled sources then. Need to record live.

    class SoundSource;

    class SoundInput : private Immovable {
    public:

        void setSource(SoundSource*) noexcept;

        SoundSource* getSource() noexcept;
        const SoundSource* getSource() const noexcept;

        /*
         * Returns whether or not the given SoundSource is a direct
         * or indirect source to this input.
         */
        bool hasDependency(const SoundSource*) const noexcept;

        /*
         * Returns the maximum number of times that a given SoundSource
         * may be queried for a single chunk of processing from this
         * input.
         * Returns 0 if the SoundSource is neither directly nor indirectly
         * connected to this input.
         * If the SoundSource is uncontrolled, it is only safe to use it as input
         * if the multiplicity is 1.
         */
        virtual size_t getMultiplicityTo(const SoundSource*) const noexcept = 0;

        /*
         * Returns true if the SoundSource is directly connected, or if all
         * intermediate SoundSources through which it is connected are also
         * realtime.
         * Returns false if the given SoundSource is not a dependency of this
         * input.
         */
        bool isRealTimeTo(const SoundSource*) const noexcept;

        // No getNextChunk(...) because its arguments may vary

    private:
        const SoundSource* m_parent;
        SoundSource* m_source;
    };

} // namespace flo::sound
