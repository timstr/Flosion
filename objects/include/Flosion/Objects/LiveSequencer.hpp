#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/SingleSoundInput.hpp>

namespace flo {

    class LiveSequencer;

    class Track {
    public:
        Track(LiveSequencer*);

        double volume() const noexcept;

        // Value must be between 0 and 1
        void setVolume(double);

        enum class Mode : std::uint8_t {
            // Silence. Recorded buffer is not modified, such that
            // recording (i.e. playing live), pausing, and then playing
            // recorded again will still repeat what was recorded live.
            Pause,

            // Audio is played and recorded in realtime from input
            // Previous recording is overwritten

            LiveOngoing,

            LiveRestarting,

            LiveOnce,

            // Whatever was last played live is repeated
            RecordedInput
        };

        Mode currentMode() const noexcept;
        Mode nextMode() const noexcept;

        void setNextMode(Mode);

        void resetNow();

        SingleSoundInput input;

        Signal<Mode> onChangeCurrentMode; // TODO: call this during renderNextChunk? Is that safe?
        Signal<Mode> onChangeNextMode;
        Signal<double> onChangeVolume;

    private:
        LiveSequencer* const m_parent;

        std::vector<SoundChunk> m_chunks;
        std::atomic<double> m_volume;

        Mode m_currentMode;
        std::atomic<Mode> m_nextMode;

        friend class LiveSequencer;
    };

    class LiveSequencerState : public SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

    private:

        std::size_t m_sequenceLength;

        std::vector<std::unique_ptr<Track>> m_tracks;

        // NOTE: this will not necessarily be a multiple of SoundChunk::size
        // since is wraps around subject to the sequence length
        std::size_t m_current_pos = 0;

        friend class LiveSequencer;
    };

    class LiveSequencer : public WithCurrentTime<Realtime<UncontrolledSoundSource<LiveSequencerState>>> {
    public:
        LiveSequencer();
        ~LiveSequencer();

        std::size_t currentPosition() const;

        void setLength(std::size_t);
        std::size_t length() const noexcept;

        double maxVolume() const noexcept;

        Track* addTrack();
        void removeTrack(Track*);

        std::vector<Track*> getTracks();

        Signal<Track*> onAddTrack;
        Signal<Track*> onRemoveTrack;

    private:
        void renderNextChunk(SoundChunk&, LiveSequencerState*) noexcept override;

        friend class Track;
    };

} // namespace flo