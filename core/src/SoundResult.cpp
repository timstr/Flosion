#include <SoundResult.hpp>

namespace flo {
    SoundResult::SoundResult(){
        addDependency(&m_input);
    }
    SoundResult::~SoundResult(){
        removeDependency(&m_input);
    }
    void SoundResult::getNextChunk(SoundChunk& chunk){
        return m_input.getNextChunkFor(chunk, this, getMonoState());
    }

    void SoundResult::reset(){
        m_input.resetStateFor(this, getMonoState());
    }

    void SoundResult::setSource(SoundSource* source) noexcept {
        m_input.setSource(source);
    }

} // namespace flo
