#include <Flosion/Objects/Melody.hpp>

namespace flo {

    MelodyState::NoteInProgress* MelodyState::addNoteInProgress(const MelodyNote* note){
        for (auto& maybeNoteInProgress: m_notesInProgress){
            if (!maybeNoteInProgress.has_value()){
                maybeNoteInProgress.emplace(note, nextAvailableInputKey());
                return &maybeNoteInProgress.value();
            }
        }
        assert(false);
        return nullptr;
    }

    void MelodyState::removeNoteInProgress(NoteInProgress* nip){
        for (auto& maybeNote : m_notesInProgress){
            if (maybeNote.has_value() && (&maybeNote.value() == nip)){
                maybeNote.reset();
                return;
            }
        }
        assert(false);
    }

    std::vector<std::optional<MelodyState::NoteInProgress>>& MelodyState::getNotesInProgress() noexcept {
        return m_notesInProgress;
    }

    void MelodyState::reset() noexcept {
        for (auto& nip : m_notesInProgress){
            nip.reset();
        }
        m_elapsedTime = 0;
    }

    std::size_t MelodyState::nextAvailableInputKey() const noexcept {
        const auto numKeys = m_notesInProgress.size();
        assert(numKeys == this->getOwner().input.numKeys());


        for (std::size_t k = 0; k < numKeys; ++k){
            bool taken = false;
            for (const auto& maybeNote : m_notesInProgress){
                if (maybeNote.has_value() && maybeNote->m_inputKey == k){
                    taken = true;
                    break;
                }
            }
            if (!taken){
                return k;
            }
        }

        // This should never happen
        assert(false);
        return static_cast<std::size_t>(-1);
    }

    MelodyState::MelodyState(SoundNode* owner, const SoundState* dependentState)
        : ConcreteSoundState(owner, dependentState)
        , m_notesInProgress(getOwner().input.numKeys()) {

    }

    void MelodyState::resizeQueue(std::size_t newSize){
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

    MelodyNote::MelodyNote(Melody* parentMelody, std::size_t startTime, std::size_t length, double frequency)
        : m_parentMelody(parentMelody)
        , m_startTime(startTime)
        , m_length(length)
        , m_frequency(frequency) {

    }

    std::size_t MelodyNote::startTime() const noexcept {
        return m_startTime;
    }

    std::size_t MelodyNote::length() const noexcept {
        return m_length;
    }

    double MelodyNote::frequency() const noexcept {
        return m_frequency;
    }

    void MelodyNote::setStartTime(std::size_t st) noexcept {
        auto l = m_parentMelody->acquireLock();
        m_startTime = st;
        m_parentMelody->updateQueueSize();
    }

    void MelodyNote::setLength(std::size_t l) noexcept {
        auto lock = m_parentMelody->acquireLock();
        m_length = l;
        m_parentMelody->updateQueueSize();
    }

    void MelodyNote::setFrequency(double f) noexcept {
        m_frequency = f;
    }

    void MelodyNoteState::reset() noexcept {
        m_currentNote = nullptr;
    }

    const MelodyNote* MelodyNoteState::note() const noexcept {
        return m_currentNote;
    }

    Melody::Melody()
        : input(this)
        , m_length(sampleFrequency * 4)
        , m_loopEnabled(true) {

    }

    MelodyNote* Melody::addNote(std::size_t startTime, std::size_t length, double frequency){
        auto lock = acquireLock();
        auto np = std::make_unique<MelodyNote>(this, startTime, length, frequency);
        auto ret = np.get();
        m_notes.push_back(std::move(np));
        updateQueueSize();
        return ret;
    }

    std::size_t Melody::numNotes() const noexcept {
        return m_notes.size();
    }

    MelodyNote* Melody::getNote(std::size_t i) noexcept {
        return const_cast<MelodyNote*>(const_cast<const Melody*>(this)->getNote(i));
    }

    const MelodyNote* Melody::getNote(std::size_t i) const noexcept {
        assert(i < m_notes.size());
        return m_notes[i].get();
    }

    void Melody::removeNote(const MelodyNote* mn){
        auto lock = acquireLock();
        const auto sameNote = [&](const std::unique_ptr<MelodyNote>& up){
            return up.get() == mn;
        };

        assert(count_if(begin(m_notes), end(m_notes), sameNote) == 1);
        auto it = find_if(begin(m_notes), end(m_notes), sameNote);
        assert(it != end(m_notes));
        m_notes.erase(it);
    }

    bool Melody::looping() const noexcept {
        return m_loopEnabled;
    }

    std::size_t Melody::length() const noexcept {
        return m_length;
    }

    void Melody::setLooping(bool l){
        if (l == m_loopEnabled){
            return;
        }
        auto lock = acquireLock();
        m_loopEnabled = l;

        // If looping was just disabled, adjust any states where the melody
        // has played past the end to play to completion, then no more
        if (!m_loopEnabled){
            for (std::size_t i = 0, iEnd = StateTable::numSlots(); i != iEnd; ++i){
                auto s = StateTable::getState<MelodyState>(i);
                s->m_elapsedTime %= m_length;
            }
        }
    }

    void Melody::setLength(std::size_t l){
        auto lock = acquireLock();
        
        // If looping is enabled and any states are playing past the end
        // of the new length, reset them
        if (l < m_length && m_loopEnabled){
            for (std::streamsize i = 0, iEnd = StateTable::numSlots(); i != iEnd; ++i){
                auto s = StateTable::getState<MelodyState>(i);
                if (s->m_elapsedTime >= l){
                    StateTable::resetState(s);
                    assert(s->m_elapsedTime == 0);
                }
            }
        }

        m_length = l;
    }

    void Melody::renderNextChunk(SoundChunk& chunk, MelodyState* state){
        chunk.silence();

        // TODO: this seems like a reasonable restriction but it can
        // probably be lifted too, once it's clear how to account
        // for the same note potentially being started multiple times
        // in the same chunk
         assert(m_length > SoundChunk::size);

        // TODO: proper mixing
        const auto attenuation = 1.0f / static_cast<float>(input.numKeys());

        // For every note that is already playing...
        for (auto& maybeNotePlaying : state->m_notesInProgress){
            if (!maybeNotePlaying.has_value()){
                continue;
            }

            auto& notePlaying = *maybeNotePlaying;

            auto noteState = input.getState(this, state, notePlaying.inputKey());

            assert(noteState->m_currentNote);
            assert(count_if(
                begin(m_notes),
                end(m_notes),
                [&](const std::unique_ptr<MelodyNote>& mnp){ return mnp.get() == noteState->m_currentNote; }
            ) == 1);

            const auto carryOver = (SoundChunk::size - (notePlaying.elapsedTime() % SoundChunk::size)) % SoundChunk::size;

            // play the end of the note's last chunk, either fully
            // or up to its end. Remove the note if it's done now
            // and skip the rest of this loop iteration
            const auto endLength = std::min(carryOver, notePlaying.remainingTime());
            for (std::size_t i = 0; i < endLength; ++i){
                chunk[i] += notePlaying.buffer()[i + SoundChunk::size - carryOver] * attenuation;
            }
            notePlaying.advance(endLength);
            if (notePlaying.remainingTime() == 0){
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
            for (std::size_t i = 0; i < beginLength; ++i){
                chunk[i + carryOver] += notePlaying.buffer()[i] * attenuation;
            }
            notePlaying.advance(beginLength);

            // if the note finishes this chunk, remove it from the queue
            if (notePlaying.remainingTime() == 0){
                state->removeNoteInProgress(&notePlaying);
                noteState->m_currentNote = nullptr;
                continue;
            }
        }

        // For every note that will start this chunk...
        for (auto& note : m_notes){
            const auto melodyTime = m_loopEnabled ? (state->m_elapsedTime % m_length) : state->m_elapsedTime;
            const auto loopAround = m_loopEnabled && (melodyTime + SoundChunk::size > m_length);
            const bool startsNow =
                (
                    // Usual case: the note starts after this chunk and before the next chunk
                    note->startTime() >= melodyTime &&
                    note->startTime() < std::min(melodyTime + SoundChunk::size, m_length)
                ) || (
                    // Corner case: the note starts during loop-around
                    loopAround && (note->startTime() < (m_length - melodyTime))
                );
            if (startsNow){
                // Make a new spot in the queue
                auto notePlaying = state->addNoteInProgress(note.get());
                
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
                const auto beginLength = std::min(SoundChunk::size - carryOver, note->length());
                for (std::size_t i = 0; i < beginLength; ++i){
                    chunk[i + carryOver] += notePlaying->buffer()[i] * attenuation;
                }

                assert(notePlaying->elapsedTime() == 0);
                notePlaying->advance(beginLength);

                // Take the note off the queue if it finished already
                if (notePlaying->remainingTime() == 0){
                    state->removeNoteInProgress(notePlaying);
                    noteState->m_currentNote = nullptr;
                    continue;
                }
            }
        }

        state->m_elapsedTime += SoundChunk::size;
    }

    double Melody::getTimeSpeed(const SoundState*) const noexcept {
        return 1.0;
    }   

    std::size_t Melody::getMaximumOverlap() const noexcept {
        // TODO: actually compute this
        // How can this be done efficiently while still
        // accounting for notes wrapping around past the end?
        // NOTE: if a note wraps around, it will add at most
        // ceil([length of the note] / [length of the melody])
        // to the overlap
        return numNotes();
    }

    void Melody::updateQueueSize(){
        auto lock = acquireLock();
        auto queueSize = getMaximumOverlap();

        // TODO: how can previous states be preserved???
        while (input.numKeys() > 0){
            input.removeKey(input.getKey(0));
        }
        for (std::size_t i = 0; i < queueSize; ++i){
            input.addKey(i);
        }

        for (std::size_t i = 0, iEnd = StateTable::numSlots(); i != iEnd; ++i){
            auto s = StateTable::getState<MelodyState>(i);
            s->resizeQueue(queueSize);
        }
    }

    Melody::Input::Input(Melody* parent)
        : MultiSoundInput(parent)
        , noteTime(this)
        , noteProgress(this)
        , noteLength(this)
        , noteFrequency(this) {

    }

    double Melody::Input::NoteFrequency::evaluate(const MelodyNoteState* state, const SoundState*) const noexcept {
        assert(state->note());
        return state->note()->frequency();
    }

    const MelodyNote* MelodyState::NoteInProgress::note() const noexcept {
        return m_note;
    }

    SoundChunk& MelodyState::NoteInProgress::buffer() noexcept {
        return m_buffer;
    }

    std::size_t MelodyState::NoteInProgress::inputKey() const noexcept {
        return m_inputKey;
    }

    std::size_t MelodyState::NoteInProgress::elapsedTime() const noexcept {
        return m_elapsedTime;
    }

    std::size_t MelodyState::NoteInProgress::remainingTime() const noexcept {
        assert(m_elapsedTime <= m_note->length());
        return m_note->length() - m_elapsedTime;
    }

    void MelodyState::NoteInProgress::advance(std::size_t samples) noexcept {
        assert(m_elapsedTime + samples <= m_note->length());
        m_elapsedTime += samples;
    }

    MelodyState::NoteInProgress::NoteInProgress(const MelodyNote* note, std::size_t inputKey)
        : m_note(note) 
        , m_inputKey(inputKey)
        , m_elapsedTime(0) {

    }

    double Melody::Input::NoteProgress::evaluate(const MelodyNoteState* state, const SoundState* context) const noexcept {
        const auto time = context->getElapsedTimeAt(getOwner());
        const auto length = static_cast<double>(state->note()->length()) / static_cast<double>(sampleFrequency);
        return time / length;
    }

    double Melody::Input::NoteLength::evaluate(const MelodyNoteState* state, const SoundState*) const noexcept {
        return static_cast<double>(state->note()->length()) / static_cast<double>(sampleFrequency);
    }

} // namespace flo
