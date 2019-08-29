#pragma once

#include <SoundInput.hpp>
#include <SoundSource.hpp>
#include <StateTable.hpp>
#include <SoundChunk.hpp>

#include <cassert>
#include <map>

namespace flo {

    template<typename StateType, typename KeyType>
    class MultiSoundInput : public Divergent<SoundInput, StateType, KeyType> {
    public:
        
        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state, const KeyType& key);
    };

} // namespace flo

#include <MultiSoundInput.tpp>
