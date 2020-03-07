#include <Flosion/Objects/LiveSequencer.hpp>

#include <Flosion/Util/Volume.hpp>

namespace flo {

    LiveSequencer::LiveSequencer() {
        // TODO: hack
        enableMonostate();

        setLength(Sample::frequency * 4);
    }

    LiveSequencer::~LiveSequencer(){
        auto s = getMonoState();
        while (s->m_tracks.size() > 0) {
            removeTrack(s->m_tracks.front().get());
        }
    }

    std::size_t LiveSequencer::currentPosition() const {
        // TODO: synchronize this, probably using atomic<std::size_t>
        return getMonoState()->m_current_pos;
    }

    void LiveSequencer::setLength(std::size_t l){
        assert(l >= SoundChunk::size);
        auto lock = acquireLock();
        auto s = getMonoState();
        if (s->m_current_pos >= l) {
            s->m_current_pos = 0;
            for (auto& t : s->m_tracks) {
                t->m_currentMode = t->m_nextMode.load(std::memory_order_relaxed);
                t->onChangeCurrentMode.broadcast(t->m_currentMode);
            }
        }
        auto nChunks = l / SoundChunk::size;
        if (l % SoundChunk::size > 0) {
            nChunks += 1;
        }
        for (auto& t : s->m_tracks) {
            t->m_chunks.resize(nChunks);
        }
        s->m_sequenceLength = l;
    }

    std::size_t LiveSequencer::length() const noexcept {
        return getMonoState()->m_sequenceLength;
    }

    double LiveSequencer::maxVolume() const noexcept {
        return 1.2;
    }

    Track* LiveSequencer::addTrack(){
        Track* tp = nullptr;
        {
            auto lock = acquireLock();
            auto s = getMonoState();
            auto t = std::make_unique<Track>(this);
            auto nChunks = s->m_sequenceLength / SoundChunk::size;
            if (s->m_sequenceLength % SoundChunk::size > 0) {
                nChunks += 1;
            }
            t->m_chunks.resize(nChunks);
            tp = t.get();
            s->m_tracks.push_back(std::move(t));
        }
        onAddTrack.broadcast(tp);
        return tp;
    }

    void LiveSequencer::removeTrack(Track* tp){
        {
            auto lock = acquireLock();
            auto s = getMonoState();

            const auto match = [&](const std::unique_ptr<Track>& t) {
                return t.get() == tp;
            };

            assert(count_if(begin(s->m_tracks), end(s->m_tracks), match) == 1);
            auto it = find_if(begin(s->m_tracks), end(s->m_tracks), match);
            assert(it != end(s->m_tracks));
            s->m_tracks.erase(it);
        }
        onRemoveTrack.broadcast(tp);
    }

    std::vector<Track*> LiveSequencer::getTracks() {
        const auto s = getMonoState();
        std::vector<Track*> ret;
        std::transform(
            begin(s->m_tracks),
            end(s->m_tracks),
            back_inserter(ret),
            [](const std::unique_ptr<Track>& t){ return t.get(); }
        );
        return ret;
    }

    void LiveSequencer::renderNextChunk(SoundChunk& chunk, LiveSequencerState* state) noexcept {
        chunk.silence();

        const auto mixAllTracks = [&](std::size_t chunkIdx, std::size_t inChunkStart, std::size_t len, std::size_t outChunkStart) {
            for (const auto& t : state->m_tracks) {
                if (t->currentMode() == Track::Mode::Pause) {
                    continue;
                }
                const auto a = static_cast<float>(util::volumeToAmplitude(t->volume(), maxVolume()));
                assert(chunkIdx < t->m_chunks.size());
                const auto& inChunk = t->m_chunks[chunkIdx];
                for (std::size_t i = 0; i < len; ++i) {
                    chunk[i + outChunkStart] += inChunk[i + inChunkStart] * a;
                }
            }
        };

        const auto swapTrackModes = [&]() {
            for (auto& t : state->m_tracks) {
                t->m_currentMode = t->m_nextMode.load(std::memory_order_relaxed);
                if (t->m_currentMode == Track::Mode::LiveOnce) {
                    t->setNextMode(Track::Mode::RecordedInput);
                } else if (t->m_currentMode == Track::Mode::LiveRestarting) {
                    t->input.resetStateFor(this, getMonoState());
                }
                t->onChangeCurrentMode.broadcast(t->m_currentMode);
            }
        };

        const auto getNextTrackChunks = [&](std::size_t chunkIdx) {
            for (const auto& t : state->m_tracks) {
                const auto m = t->currentMode();
                if (m == Track::Mode::RecordedInput || m == Track::Mode::Pause) {
                    continue;
                }
                assert(chunkIdx < t->m_chunks.size());
                auto& inChunk = t->m_chunks[chunkIdx];
                inChunk.silence();
                t->input.getNextChunkFor(inChunk, this, state);
            }
        };

        // While out chunk is not full:
        auto samplesWritten = std::size_t{ 0 };
        while (samplesWritten < SoundChunk::size) {
            // Write portion of current track chunks to out chunk
            // (portion is the lesser of remaining chunk length and remaining sequence length)
            assert(state->m_current_pos <= state->m_sequenceLength);
            if (state->m_current_pos < state->m_sequenceLength) {
                auto inChunkIdx = state->m_current_pos / SoundChunk::size;
                auto inChunkOffset = state->m_current_pos % SoundChunk::size;
                auto inChunkRemainder = SoundChunk::size - inChunkOffset;
                auto sequenceRemainder = state->m_sequenceLength - state->m_current_pos;
                auto inChunkPortion = std::min(inChunkRemainder, sequenceRemainder);
                mixAllTracks(inChunkIdx, inChunkOffset, inChunkPortion, samplesWritten);
                samplesWritten += inChunkPortion;
                state->m_current_pos += inChunkPortion;
            }

            // if end of sequence is reached, set position to zero and swap tracks to next mode
            assert(samplesWritten <= SoundChunk::size);
            assert(state->m_current_pos <= state->m_sequenceLength);
            if (state->m_current_pos == state->m_sequenceLength) {
                state->m_current_pos = 0;
                swapTrackModes();
            }

            // Get next chunk for each track
            assert(state->m_current_pos < state->m_sequenceLength);
            auto inChunkIdx = state->m_current_pos / SoundChunk::size;
            getNextTrackChunks(inChunkIdx);

            // Write portion of current track chunks to out chunk
            // (portion is the lesser of the remaining outChunk length and remaining sequence length)
            auto inChunkRemainder = SoundChunk::size - samplesWritten;
            auto sequenceRemainder = state->m_sequenceLength - state->m_current_pos;
            auto inChunkPortion = std::min(inChunkRemainder, sequenceRemainder);

            if (inChunkPortion == 0) {
                break;
            }
            mixAllTracks(inChunkIdx, 0, inChunkPortion, samplesWritten);
            samplesWritten += inChunkPortion;
            state->m_current_pos += inChunkPortion;
        }
        assert(samplesWritten == SoundChunk::size);
    }

    Track::Track(LiveSequencer* ls)
        : input(ls)
        , m_parent(ls)
        , m_volume(0.8)
        , m_currentMode(Mode::Pause)
        , m_nextMode(Mode::Pause) {

    }

    double Track::volume() const noexcept {
        return m_volume.load(std::memory_order_relaxed);
    }

    void Track::setVolume(double v) {
        v = std::clamp(v, 0.0, m_parent->maxVolume());
        if (std::abs(v - m_volume) > 1e-6) {
            m_volume.store(v, std::memory_order_relaxed);
            onChangeVolume.broadcast(m_volume);
        }
    }

    Track::Mode Track::currentMode() const noexcept {
        return m_currentMode;
    }

    Track::Mode Track::nextMode() const noexcept {
        return m_nextMode.load(std::memory_order_relaxed);
    }

    void Track::setNextMode(Mode m) {
        m_nextMode.store(m, std::memory_order_relaxed);
        onChangeNextMode.broadcast(m);
    }

    void Track::resetNow(){
        assert(m_parent);
        auto lock = m_parent->acquireLock();
        input.resetStateFor(m_parent, m_parent->getMonoState());
    }

    void LiveSequencerState::reset() noexcept {
        m_current_pos = 0;
        // NOTE: tracks are not modified, they just restart from beginning
    }

}
