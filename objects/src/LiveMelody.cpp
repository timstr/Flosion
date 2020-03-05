#include <Flosion/Objects/LiveMelody.hpp>

#include <algorithm>

namespace flo {

    LiveMelodyState::NoteInProgress* LiveMelodyState::addNoteInProgress(const LiveMelodyNote* note) {
        for (auto& maybeNoteInProgress : m_notesInProgress) {
            if (!maybeNoteInProgress.has_value()) {
                maybeNoteInProgress.emplace(note, nextAvailableInputKey());
                return &maybeNoteInProgress.value();
            }
        }
        //assert(false);
        return nullptr;
    }

    void LiveMelodyState::removeNoteInProgress(NoteInProgress* nip) {
        for (auto& maybeNote : m_notesInProgress) {
            if (maybeNote.has_value() && (&maybeNote.value() == nip)) {
                maybeNote.reset();
                return;
            }
        }
        assert(false);
    }

    std::vector<std::optional<LiveMelodyState::NoteInProgress>>& LiveMelodyState::getNotesInProgress() noexcept {
        return m_notesInProgress;
    }

    void LiveMelodyState::reset() noexcept {
        for (auto& nip : m_notesInProgress) {
            nip.reset();
        }
        m_elapsedTime = 0;
    }

    std::size_t LiveMelodyState::nextAvailableInputKey() const noexcept {
        const auto numKeys = m_notesInProgress.size();
        assert(numKeys == this->getOwner().input.numKeys());


        for (std::size_t k = 0; k < numKeys; ++k) {
            bool taken = false;
            for (const auto& maybeNote : m_notesInProgress) {
                if (maybeNote.has_value() && maybeNote->m_inputKey == k) {
                    taken = true;
                    break;
                }
            }
            if (!taken) {
                return k;
            }
        }

        // This should never happen
        assert(false);
        return static_cast<std::size_t>(-1);
    }

    LiveMelodyState::LiveMelodyState(SoundNode* owner, const SoundState* dependentState)
        : ConcreteSoundState(owner, dependentState)
        , m_notesInProgress(getOwner().input.numKeys()) {

    }

    void LiveMelodyState::resizeQueue(std::size_t newSize) {
        // TODO: If the queue must be shrunk, there
        // will be times that some notes which are
        // already playing must be dropped. Should
        // the queue never shrink? Should some
        // policy be chosen for discarding notes?
        // It would be undesirable to discard all notes
        // that are currently playing when another
        // note was removed/resized.
        // TODO: how to rearrange input keys?
        // How to preserve states of notes whose keys
        // are being removed?

        auto newQueue = std::vector<std::optional<NoteInProgress>>(newSize);

        // TODO: preserve as many notes as possible
        // Lazy option for now: just drop every note

        swap(newQueue, m_notesInProgress);
    }

    LiveMelodyNote::LiveMelodyNote(LiveMelody* parentMelody, std::size_t startTime, double frequency)
        : m_parentMelody(parentMelody)
        , m_startTime(startTime)
        , m_minLength(parentMelody->noteReleaseTime())
        , m_firstPlay(true)
        , m_frequency(frequency) {

    }

    std::size_t LiveMelodyNote::startTime() const noexcept {
        return m_startTime;
    }

    std::size_t LiveMelodyNote::minLength() const noexcept {
        return m_minLength;
    }

    double LiveMelodyNote::frequency() const noexcept {
        return m_frequency;
    }

    bool LiveMelodyNote::firstPlay() const noexcept {
        return m_firstPlay;
    }

    void LiveMelodyNote::extend(std::size_t samples){
        assert(m_firstPlay);
        m_minLength += samples;
    }

    void LiveMelodyNote::stop(){
        assert(m_firstPlay);
        m_firstPlay = false;
    }

    /*void LiveMelodyNote::setStartTime(std::size_t st) noexcept {
        auto l = m_parentLiveMelody->acquireLock();
        m_startTime = st;
        m_parentLiveMelody->updateQueueSize();
    }

    void LiveMelodyNote::setLength(std::size_t l) noexcept {
        auto lock = m_parentLiveMelody->acquireLock();
        m_length = l;
        m_parentLiveMelody->updateQueueSize();
    }

    void LiveMelodyNote::setFrequency(double f) noexcept {
        m_frequency = f;
    }*/

    void LiveMelodyNoteState::reset() noexcept {
        m_currentNote = nullptr;
    }

    const LiveMelodyNote* LiveMelodyNoteState::note() const noexcept {
        return m_currentNote;
    }

    LiveMelody::LiveMelody()
        : input(this)
        , m_length(sampleFrequency * 4)
        , m_noteReleaseTime(Sample::frequency / 4)
        , m_loopEnabled(true) {

        // TODO: AAAAAAAAAAAAAaaa hack!
        StateTable::enableMonostate();

        updateQueueSize();
    }

    LiveMelodyNote* LiveMelody::startNote(double frequency){
        auto lock = acquireLock();

        auto state = getMonoState();

        auto now = state->m_elapsedTime;
        auto np = std::make_unique<LiveMelodyNote>(this, now, frequency);
        auto ret = np.get();
        m_notes.push_back(std::move(np));

        auto nip = state->addNoteInProgress(ret);

        if (!nip) {
            m_notes.pop_back();
            return nullptr;
        }

        assert(nip->remainingTime() > 0);
        input.resetStateFor(this, state, nip->inputKey());
        auto noteState = input.getState(this, state, nip->inputKey());
        noteState->m_currentNote = ret;

        return ret;
    }

    std::vector<LiveMelodyNote*> LiveMelody::getNotes() const noexcept {
        std::vector<LiveMelodyNote*> ret;
        transform(
            begin(m_notes),
            end(m_notes),
            back_inserter(ret),
            [](const std::unique_ptr<LiveMelodyNote>& np) { return np.get();  }
        );
        return ret;
    }

    void LiveMelody::stopNote(LiveMelodyNote* np){
        auto lock = acquireLock();

        assert(count_if(
            begin(m_notes),
            end(m_notes),
            [&](const std::unique_ptr<LiveMelodyNote>& up) { return up.get() == np; }
        ) == 1);

        np->stop();
    }

    /*LiveMelodyNote* LiveMelody::addNote(std::size_t startTime, std::size_t length, double frequency) {
        auto lock = acquireLock();
        auto np = std::make_unique<LiveMelodyNote>(this, startTime, length, frequency);
        auto ret = np.get();
        m_notes.push_back(std::move(np));
        updateQueueSize();
        return ret;
    }*/

    std::size_t LiveMelody::maxNumNotes() const noexcept {
        // return m_notes.size();
        return 2;
    }

    std::size_t LiveMelody::noteReleaseTime() const noexcept {
        return m_noteReleaseTime;
    }

    /*LiveMelodyNote* LiveMelody::getNote(std::size_t i) noexcept {
        return const_cast<LiveMelodyNote*>(const_cast<const LiveMelody*>(this)->getNote(i));
    }

    const LiveMelodyNote* LiveMelody::getNote(std::size_t i) const noexcept {
        assert(i < m_notes.size());
        return m_notes[i].get();
    }

    void LiveMelody::removeNote(const LiveMelodyNote* mn) {
        auto lock = acquireLock();
        const auto sameNote = [&](const std::unique_ptr<LiveMelodyNote>& up) {
            return up.get() == mn;
        };

        assert(count_if(begin(m_notes), end(m_notes), sameNote) == 1);
        auto it = find_if(begin(m_notes), end(m_notes), sameNote);
        assert(it != end(m_notes));
        m_notes.erase(it);
    }*/

    bool LiveMelody::looping() const noexcept {
        return m_loopEnabled;
    }

    std::size_t LiveMelody::length() const noexcept {
        return m_length;
    }

    void LiveMelody::setLooping(bool l) {
        if (l == m_loopEnabled) {
            return;
        }
        auto lock = acquireLock();
        m_loopEnabled = l;

        // If looping was just disabled, adjust any states where the melody
        // has played past the end to play to completion, then no more
        if (!m_loopEnabled) {
            for (std::size_t i = 0, iEnd = StateTable::numSlots(); i != iEnd; ++i) {
                auto s = StateTable::getState<LiveMelodyState>(i);
                s->m_elapsedTime %= m_length;
            }
        }
    }

    void LiveMelody::setLength(std::size_t l) {
        auto lock = acquireLock();

        // If looping is enabled and any states are playing past the end
        // of the new length, reset them
        if (l < m_length && m_loopEnabled) {
            for (std::streamsize i = 0, iEnd = StateTable::numSlots(); i != iEnd; ++i) {
                auto s = StateTable::getState<LiveMelodyState>(i);
                if (s->m_elapsedTime >= l) {
                    StateTable::resetState(s);
                    assert(s->m_elapsedTime == 0);
                }
            }
        }

        m_length = l;
    }

    void LiveMelody::renderNextChunk(SoundChunk& chunk, LiveMelodyState* state) {
        chunk.silence();

        // TODO: this seems like a reasonable restriction but it can
        // probably be lifted too, once it's clear how to account
        // for the same note potentially being started multiple times
        // in the same chunk
        assert(m_length > SoundChunk::size);

        // TODO: proper mixing
        const auto attenuation = 1.0f / static_cast<float>(input.numKeys());

        // extend all notes that are playing for the first time
        for (auto& note : m_notes) {
            if (note->firstPlay()) {
                note->extend(SoundChunk::size);
            }
        }

        // For every note that is already playing...
        for (auto& maybeNotePlaying : state->m_notesInProgress) {
            if (!maybeNotePlaying.has_value()) {
                continue;
            }

            auto& notePlaying = *maybeNotePlaying;

            auto noteState = input.getState(this, state, notePlaying.inputKey());

            assert(noteState->m_currentNote);
            assert(count_if(
                begin(m_notes),
                end(m_notes),
                [&](const std::unique_ptr<LiveMelodyNote>& mnp) { return mnp.get() == noteState->m_currentNote; }
            ) == 1);

            const auto carryOver = (SoundChunk::size - (notePlaying.elapsedTime() % SoundChunk::size)) % SoundChunk::size;

            // play the end of the note's last chunk, either fully
            // or up to its end. Remove the note if it's done now
            // and skip the rest of this loop iteration
            const auto endLength = std::min(carryOver, notePlaying.remainingTime());
            for (std::size_t i = 0; i < endLength; ++i) {
                chunk[i] += notePlaying.buffer()[i + SoundChunk::size - carryOver] * attenuation;
            }
            notePlaying.advance(endLength);
            if (notePlaying.remainingTime() == 0) {
                state->removeNoteInProgress(&notePlaying);
                noteState->m_currentNote = nullptr;
                continue;
            }

            // get the next chunk of the note
            const auto noteTimeOffset = carryOver;
            state->adjustTime(static_cast<std::uint32_t>(noteTimeOffset));
            input.getNextChunkFor(notePlaying.buffer(), this, state, notePlaying.inputKey());

            // play the note until the end of the chunk, or until the
            // end of the note
            const auto beginLength = std::min(SoundChunk::size - carryOver, notePlaying.remainingTime());
            for (std::size_t i = 0; i < beginLength; ++i) {
                chunk[i + carryOver] += notePlaying.buffer()[i] * attenuation;
            }
            notePlaying.advance(beginLength);

            // if the note finishes this chunk, remove it from the queue
            if (notePlaying.remainingTime() == 0) {
                state->removeNoteInProgress(&notePlaying);
                noteState->m_currentNote = nullptr;
                continue;
            }
        }

        // For every note that will start this chunk...
        for (auto& note : m_notes) {
            if (note->firstPlay()) {
                continue;
            }

            const auto melodyTime = m_loopEnabled ? (state->m_elapsedTime % m_length) : state->m_elapsedTime;
            const auto loopAround = m_loopEnabled && (melodyTime + SoundChunk::size > m_length);
            const bool startsNow = (
                // Usual case: the note starts after this chunk and before the next chunk
                note->startTime() >= melodyTime &&
                note->startTime() < std::min(melodyTime + SoundChunk::size, m_length)
            ) || (
                // Corner case: the note starts during loop-around
                loopAround && (note->startTime() < (m_length - melodyTime))
            );

            if (startsNow) {
                // Make a new spot in the queue
                auto notePlaying = state->addNoteInProgress(note.get());

                if (!notePlaying) {
                    break;
                }

                // Start sample within the current chunk
                // Also the number of samples from note's chunk that will be played next time
                const auto carryOver = loopAround ?
                    note->startTime() + SoundChunk::size - (melodyTime % SoundChunk::size) :
                    note->startTime() - melodyTime;
                assert(carryOver <= SoundChunk::size);

                // get the first chunk of the note
                assert(notePlaying->remainingTime() > 0);
                input.resetStateFor(this, state, notePlaying->inputKey());
                auto noteState = input.getState(this, state, notePlaying->inputKey());
                noteState->m_currentNote = note.get();
                state->adjustTime(static_cast<std::uint32_t>(carryOver));
                input.getNextChunkFor(notePlaying->buffer(), this, state, notePlaying->inputKey());

                // play the note until the end of this chunk, or until
                // the end of the note, whichever comes first
                const auto beginLength = std::min(SoundChunk::size - carryOver, note->minLength());
                for (std::size_t i = 0; i < beginLength; ++i) {
                    chunk[i + carryOver] += notePlaying->buffer()[i] * attenuation;
                }

                assert(notePlaying->elapsedTime() == 0);
                notePlaying->advance(beginLength);

                // Take the note off the queue if it finished already
                if (notePlaying->remainingTime() == 0) {
                    state->removeNoteInProgress(notePlaying);
                    noteState->m_currentNote = nullptr;
                    continue;
                }
            }
        }

        state->m_elapsedTime += SoundChunk::size;
    }

    double LiveMelody::getTimeSpeed(const SoundState*) const noexcept {
        return 1.0;
    }

    std::size_t LiveMelody::getMaximumOverlap() const noexcept {
        // TODO: actually compute this
        // How can this be done efficiently while still
        // accounting for notes wrapping around past the end?
        // NOTE: if a note wraps around, it will add at most
        // ceil([length of the note] / [length of the melody])
        // to the overlap
        return maxNumNotes();
    }

    void LiveMelody::updateQueueSize() {
        auto lock = acquireLock();
        auto queueSize = getMaximumOverlap();

        // TODO: how can previous states be preserved???
        while (input.numKeys() > 0) {
            input.removeKey(input.getKey(0));
        }
        for (std::size_t i = 0; i < queueSize; ++i) {
            input.addKey(i);
        }

        for (std::size_t i = 0, iEnd = StateTable::numSlots(); i != iEnd; ++i) {
            auto s = StateTable::getState<LiveMelodyState>(i);
            s->resizeQueue(queueSize);
        }
    }

    LiveMelody::Input::Input(LiveMelody* parent)
        : MultiSoundInput(parent)
        , noteTime(this)
        , noteProgress(this)
        , noteLength(this)
        , noteFrequency(this) {

    }

    double LiveMelody::Input::NoteFrequency::evaluate(const LiveMelodyNoteState* state, const SoundState*) const noexcept {
        assert(state->note());
        return state->note()->frequency();
    }

    const LiveMelodyNote* LiveMelodyState::NoteInProgress::note() const noexcept {
        return m_note;
    }

    SoundChunk& LiveMelodyState::NoteInProgress::buffer() noexcept {
        return m_buffer;
    }

    std::size_t LiveMelodyState::NoteInProgress::inputKey() const noexcept {
        return m_inputKey;
    }

    std::size_t LiveMelodyState::NoteInProgress::elapsedTime() const noexcept {
        return m_elapsedTime;
    }

    std::size_t LiveMelodyState::NoteInProgress::remainingTime() const noexcept {
        assert(m_elapsedTime <= m_note->minLength());
        return m_note->minLength() - m_elapsedTime;
    }

    void LiveMelodyState::NoteInProgress::advance(std::size_t samples) noexcept {
        assert(m_elapsedTime + samples <= m_note->minLength());
        m_elapsedTime += samples;
    }

    LiveMelodyState::NoteInProgress::NoteInProgress(const LiveMelodyNote* note, std::size_t inputKey)
        : m_note(note)
        , m_inputKey(inputKey)
        , m_elapsedTime(0) {

    }

    double LiveMelody::Input::NoteProgress::evaluate(const LiveMelodyNoteState* state, const SoundState* context) const noexcept {
        const auto time = context->getElapsedTimeAt(getOwner());
        const auto length = static_cast<double>(state->note()->minLength()) / static_cast<double>(sampleFrequency);
        return time / length;
    }

    double LiveMelody::Input::NoteLength::evaluate(const LiveMelodyNoteState* state, const SoundState*) const noexcept {
        return static_cast<double>(state->note()->minLength()) / static_cast<double>(sampleFrequency);
    }

} // namespace flo
