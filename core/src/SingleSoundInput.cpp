#include <Flosion/Core/SingleSoundInput.hpp>
#include <Flosion/Core/SoundSource.hpp>

#include <cassert>

namespace flo {

    SingleSoundInput::SingleSoundInput(SoundNode* parent)
        : m_parent(parent) {
        if (m_parent){
            m_parent->addDependency(this);
        }
    }

    SingleSoundInput::~SingleSoundInput(){
        if (m_parent){
            m_parent->removeDependency(this);
        }
    }

    void SingleSoundInput::getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state){
        assert(node->hasDirectDependency(this));
        if (getSource()){
            getSource()->getNextChunkFor(chunk, this, getState(node, state));
        } else {
            chunk.silence();
        }
    }

} // namespace flo
