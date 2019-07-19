#include <SingleSoundInput.hpp>

namespace flo {

    SingleSoundInput::SingleSoundInput()
        : SoundInput(Propagation::Singular, std::make_unique<ConcreteStateAllocator<EmptySoundState>>()) {
    
    }

    void SingleSoundInput::getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state){
        auto src = getSource();
        if (src){
            // TODO: look up own state or map state
        } else {
            chunk.silence();
        }
    }

} // namespace flo
