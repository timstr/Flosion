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

} // namespace flo
