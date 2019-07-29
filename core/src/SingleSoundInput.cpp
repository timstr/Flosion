#include <SingleSoundInput.hpp>
#include <SoundSource.hpp>

#include <cassert>

namespace flo {

    void SingleSoundInput::getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state){
        if (getSource()){
            getSource()->getNextChunkFor(chunk, this, getState(node, state));
        } else {
            chunk.silence();
        }
    }

} // namespace flo
