#pragma once

#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/SoundChunk.hpp>

namespace flo {

    class SingleSoundInput : public Realtime<Singular<SoundInput, EmptySoundState>> {
    public:
        SingleSoundInput(SoundNode* parent);

        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state);
    };

} // namespace flo
