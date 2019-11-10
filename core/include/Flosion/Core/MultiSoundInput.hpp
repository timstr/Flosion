#pragma once

#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/StateTable.hpp>
#include <Flosion/Core/SoundChunk.hpp>

#include <cassert>
#include <map>

namespace flo {

    template<typename StateType, typename KeyType>
    class MultiSoundInput : public Realtime<Divergent<SoundInput, StateType, KeyType>> {
    public:
        MultiSoundInput(SoundNode* parent);
        
        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state, const KeyType& key);
    };

} // namespace flo

#include <Flosion/Core/MultiSoundInput.tpp>
