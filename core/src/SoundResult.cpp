#include <SoundResult.hpp>

namespace flo {

    SoundResult::SoundResult()
        : OSoundNode(Propagation::Singular){
    
        m_input.addStateFor(this, nullptr);
    }

    SoundResult::~SoundResult(){
        m_input.removeStateFor(this, nullptr);
    }

    void SoundResult::getNextChunk(SoundChunk& chunk){
        m_input.getNextChunkFor(chunk, this, nullptr);
    }

    void SoundResult::setSource(SoundSource* src) noexcept {
        m_input.setSource(src);
    }

} // namespace flo
