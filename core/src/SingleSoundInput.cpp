#include <SingleSoundInput.hpp>

#include <cassert>

namespace flo {

    void SingleSoundInput::getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state){
        if (getSource()){
            getSource()->getNextChunkFor(chunk, getState(node, state));
        } else {
            chunk.silence();
        }
    }

} // namespace flo
