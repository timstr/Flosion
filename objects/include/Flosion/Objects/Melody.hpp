#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/MultiSoundInput.hpp>

namespace flo {

    class Melody;
    class MelodyNote;

    // The state of the Melody object
    class MelodyState : public ConcreteSoundState<Melody> {
    public:
        MelodyState(SoundNode* owner, const SoundState* dependentState);

        // Resizes the queue of notes in progress.
        // Maybe some notes will be dropped. Who can say?
        void resizeQueue(std::size_t);

        class NoteInProgress {
        public:
            NoteInProgress(const MelodyNote* note, std::size_t inputKey);

            const MelodyNote* note() const noexcept;

            SoundChunk& buffer() noexcept;

            std::size_t inputKey() const noexcept;

            std::size_t elapsedTime() const noexcept;

            std::size_t remainingTime() const noexcept;

            void advance(std::size_t samples) noexcept;

        private:
            const MelodyNote* const m_note;
            std::size_t m_inputKey;
            std::size_t m_elapsedTime;
            SoundChunk m_buffer;

            friend class MelodyState;
        };

        NoteInProgress* addNoteInProgress(const MelodyNote*);

        void removeNoteInProgress(NoteInProgress*);

        std::vector<std::optional<NoteInProgress>>& getNotesInProgress() noexcept;

        void reset() noexcept override;

    private:

        std::size_t nextAvailableInputKey() const noexcept;

        // NOTE: this gets resized by the Melody itself when needed
        std::vector<std::optional<NoteInProgress>> m_notesInProgress;

        std::size_t m_elapsedTime = 0;

        friend class Melody;
    };



    // A note in a Melody
    class MelodyNote {
    public:
        MelodyNote(Melody* parentMelody, std::size_t startTime, std::size_t length, double frequency);
        MelodyNote(MelodyNote&&) = delete;
        MelodyNote(const MelodyNote&) = delete;
        MelodyNote& operator=(MelodyNote&&) = delete;
        MelodyNote& operator=(const MelodyNote&) = delete;
        ~MelodyNote() = default;

        std::size_t startTime() const noexcept;
        std::size_t length() const noexcept;
        double frequency() const noexcept;

        // TODO: changing the timing of a note changes how big of a queue
        // the Melody object needs. This needs to be carefully controlled
        // or accounted for during sound processing time.
        // TODO: acquire lock to prevent race condition during sound processing
        void setStartTime(std::size_t) noexcept;
        void setLength(std::size_t) noexcept;
        void setFrequency(double) noexcept;

    private:
        // The Melody to which the note belongs
        Melody* const m_parentMelody;

        // The start time of the note, in samples
        std::size_t m_startTime;

        // The length of the note, in samples
        std::size_t m_length;

        // The frequency of the note, in Hertz
        double m_frequency;

        // TODO: variable frequency, as a function of time

        // TODO: frequency as an integer ratio of another note's frequency

        // TODO: custom per-note attributes
    };



    // The state of the MultiSoundInput in a Melody object
    class MelodyNoteState : public SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

        const MelodyNote* note() const noexcept;

    private:
        const MelodyNote* m_currentNote = nullptr;

        friend class Melody;
    };



    // A sequence of notes, much like a midi sequence
    class Melody : public WithCurrentTime<OutOfSync<ControlledSoundSource<MelodyState>>> {
    public:
        Melody();

        // TODO

        MelodyNote* addNote(std::size_t startTime, std::size_t length, double frequency);
        std::size_t numNotes() const noexcept;
        MelodyNote* getNote(std::size_t) noexcept;
        const MelodyNote* getNote(std::size_t) const noexcept;
        void removeNote(const MelodyNote*);

        // TODO: maximum note release time, i.e. amount of time
        // that notes are allowed to keep playing after their length
        // has been exceeded in the Melody. Useful for things like
        // ADSR, reverberations, etc

        bool looping() const noexcept;

        void setLooping(bool);

        // The MultiSoundInput which is called upon for playing notes
        class Input : public MultiSoundInput<MelodyNoteState, std::size_t> {
        public:
            CurrentTime noteTime;

            class NoteProgress : public SoundNumberSource<Input> {
            public:
                using SoundNumberSource::SoundNumberSource;

            private:
                double evaluate(const MelodyNoteState*, const SoundState*) const noexcept override;
            } noteProgress;
            
            class NoteLength : public SoundNumberSource<Input> {
            public:
                using SoundNumberSource::SoundNumberSource;

            private:
                double evaluate(const MelodyNoteState*, const SoundState*) const noexcept override;
            } noteLength;

            class NoteFrequency : public SoundNumberSource<Input> {
            public:
                using SoundNumberSource::SoundNumberSource;

            private:
                double evaluate(const MelodyNoteState*, const SoundState*) const noexcept override;
            } noteFrequency;

        private:
            Input(Melody* parent);

            friend class Melody;
        } input;

    private:
        void renderNextChunk(SoundChunk&, MelodyState*) override;

        double getTimeSpeed(const SoundState*) const noexcept override;

        std::vector<std::unique_ptr<MelodyNote>> m_notes;

        std::size_t m_length;

        bool m_loopEnabled;

        // TODO: add limit to maximum overlap
        // It will be easy to change and expand, if the user wants to,
        // and will serve as more of a warning than a safety feature
        // to check whether the user really wants that many notes
        // playing at the same time.
        // std::size_t m_overlapLimit;

        // computes the maximum number of notes that will
        // ever be playing at the same time
        std::size_t getMaximumOverlap() const noexcept;

        // Allocates or deallocates queue space in each state as
        // required by the current number and timing of notes.
        // To be called whenever notes are modified
        void updateQueueSize();

        friend class MelodyNote;
    };

}
