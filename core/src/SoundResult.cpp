#include <SoundResult.hpp>

#include <cassert>

namespace flo {

    SoundResult::SoundResult(){
        addDependency(&m_input);
    }
    SoundResult::~SoundResult(){
        removeDependency(&m_input);
    }
    void SoundResult::getNextChunk(SoundChunk& chunk){
        // Acquire read lock to prevent race conditions
        auto lock = std::shared_lock{m_mutex};
        m_input.getNextChunkFor(chunk, this, getMonoState());
    }

    void SoundResult::reset(){
        m_input.resetStateFor(this, getMonoState());
    }

    void SoundResult::setSource(SoundSource* source) noexcept {
        m_input.setSource(source);
    }

    SingleSoundInput& SoundResult::getInput() noexcept {
        return m_input;
    }

    const SingleSoundInput & SoundResult::getInput() const noexcept {
        return m_input;
    }

    void SoundResult::findDependentSoundResults(std::vector<SoundResult*>& soundResults) noexcept {
        soundResults.push_back(this);
        assert(getDirectDependents().size() == 0);
    }

} // namespace flo
