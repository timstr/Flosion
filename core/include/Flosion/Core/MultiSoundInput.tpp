#include "MultiSoundInput.hpp"
namespace flo {

    template<typename StateType, typename KeyType>
    MultiSoundInput<StateType, KeyType>::MultiSoundInput(SoundNode* parent){
        if (parent){
            parent->addDependency(this);
        }
    }

    template<typename StateType, typename KeyType>
    inline void MultiSoundInput<StateType, KeyType>::getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state, const KeyType& key){
        assert(node->hasDirectDependency(this));
        if (const auto src = this->getSource()){
            src->getNextChunkFor(chunk, this, this->getState(node, state, key));
        } else {
            chunk.silence();
        }
    }

} // namespace flo 
