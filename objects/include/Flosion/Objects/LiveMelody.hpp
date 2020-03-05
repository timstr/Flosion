#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/MultiSoundInput.hpp>

namespace flo {

    // TODO: share most of this code with Melody

    // TODO: add a bunch of Signals for subscribing to any important sorts of changes

    // TODO: note release time (which is also minimum note time)

    class LiveMelody;
    class LiveMelodyNote;

    // The state of the Melody object
    class LiveMelodyState : public ConcreteSoundState<LiveMelody> {
    public:
        LiveMelodyState(SoundNode* owner, const SoundState* dependentState);

        // Resizes the queue of notes in progress.
        // Maybe some notes will be dropped. Who can say?
        void resizeQueue(std::size_t);

        class NoteInProgress {
        public:
            NoteInProgress(const LiveMelodyNote* note, std::size_t inputKey);

            const LiveMelodyNote* note() const noexcept;

            SoundChunk& buffer() noexcept;

            std::size_t inputKey() const noexcept;

            std::size_t elapsedTime() const noexcept;

            std::size_t remainingTime() const noexcept;

            void advance(std::size_t samples) noexcept;

        private:
            const LiveMelodyNote* const m_note;
            std::size_t m_inputKey;
            std::size_t m_elapsedTime;
            SoundChunk m_buffer;

            friend class LiveMelodyState;
        };

        NoteInProgress* addNoteInProgress(const LiveMelodyNote*);

        void removeNoteInProgress(NoteInProgress*);

        std::vector<std::optional<NoteInProgress>>& getNotesInProgress() noexcept;

        void reset() noexcept override;

    private:

        std::size_t nextAvailableInputKey() const noexcept;

        // NOTE: this gets resized by the Melody itself when needed
        std::vector<std::optional<NoteInProgress>> m_notesInProgress;

        std::size_t m_elapsedTime = 0;

        friend class LiveMelody;
    };



    // A note in a Melody

    class LiveMelodyNote {
    public:
        LiveMelodyNote(LiveMelody* parentMelody, std::size_t startTime, double frequency);
        LiveMelodyNote(LiveMelodyNote&&) = delete;
        LiveMelodyNote(const LiveMelodyNote&) = delete;
        LiveMelodyNote& operator=(LiveMelodyNote&&) = delete;
        LiveMelodyNote& operator=(const LiveMelodyNote&) = delete;
        ~LiveMelodyNote() = default;

        std::size_t startTime() const noexcept;
        std::size_t minLength() const noexcept;
        double frequency() const noexcept;

        bool firstPlay() const noexcept;

    private:

        void extend(std::size_t samples);

        void stop();

        friend class LiveMelody;

    private:
        // The Melody to which the note belongs
        LiveMelody* const m_parentMelody;

        // TODO
        // The start time of the note, in samples
        std::size_t m_startTime;

        // TODO
        std::size_t m_minLength;

        // TODO
        bool m_firstPlay;

        // The frequency of the note, in Hertz
        double m_frequency;
    };



    // The state of the MultiSoundInput in a Melody object
    class LiveMelodyNoteState : public SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

        const LiveMelodyNote* note() const noexcept;

    private:
        const LiveMelodyNote* m_currentNote = nullptr;

        friend class LiveMelody;
    };



    // A sequence of notes, much like a midi sequence
    class LiveMelody : public WithCurrentTime<OutOfSync<UncontrolledSoundSource<LiveMelodyState>>> {
    public:
        LiveMelody();


        LiveMelodyNote* startNote(double frequency);
        std::vector<LiveMelodyNote*> getNotes() const noexcept;
        void stopNote(LiveMelodyNote*);

        std::size_t maxNumNotes() const noexcept;

        std::size_t noteReleaseTime() const noexcept;

        bool looping() const noexcept;
        std::size_t length() const noexcept;

        void setLooping(bool);
        void setLength(std::size_t);

        // The MultiSoundInput which is called upon for playing notes
        class Input : public MultiSoundInput<LiveMelodyNoteState, std::size_t> {
        public:
            CurrentTime noteTime;

            class NoteProgress : public SoundNumberSource<Input> {
            public:
                using SoundNumberSource::SoundNumberSource;

            private:
                double evaluate(const LiveMelodyNoteState*, const SoundState*) const noexcept override;
            } noteProgress;

            class NoteLength : public SoundNumberSource<Input> {
            public:
                using SoundNumberSource::SoundNumberSource;

            private:
                double evaluate(const LiveMelodyNoteState*, const SoundState*) const noexcept override;
            } noteLength;

            class NoteFrequency : public SoundNumberSource<Input> {
            public:
                using SoundNumberSource::SoundNumberSource;

            private:
                double evaluate(const LiveMelodyNoteState*, const SoundState*) const noexcept override;
            } noteFrequency;

        private:
            Input(LiveMelody* parent);

            friend class LiveMelody;
        } input;

    private:
        void renderNextChunk(SoundChunk&, LiveMelodyState*) override;

        double getTimeSpeed(const SoundState*) const noexcept override;

        std::vector<std::unique_ptr<LiveMelodyNote>> m_notes;

        std::size_t m_length;

        bool m_loopEnabled;

        std::size_t m_noteReleaseTime;

        // computes the maximum number of notes that will
        // ever be playing at the same time
        std::size_t getMaximumOverlap() const noexcept;

        // Allocates or deallocates queue space in each state as
        // required by the current number and timing of notes.
        // To be called whenever notes are modified
        void updateQueueSize();

        friend class LiveMelodyNote;
    };

}
