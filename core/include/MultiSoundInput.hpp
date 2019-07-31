#pragma once

#include <SoundInput.hpp>
#include <StateTable.hpp>
#include <SoundChunk.hpp>

#include <map>

namespace flo {

    template<typename StateType, typename KeyType>
    class MultiSoundInput final : public Divergent<SoundInput, StateType, KeyType> {
    public:
        
        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state, const KeyType& key);
    };

    // TODO: move definitions below this comment to their own .tpp file

    template<typename StateType, typename KeyType>
    inline void MultiSoundInput<StateType, KeyType>::getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state, const KeyType& key){
        if (getSource()){
            getSource->getNextChunkFor(chunk, this, getState(node, state, key));
        } else {
            chunk.silence();
        }
    }

} // namespace flo
