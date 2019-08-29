#pragma once

#include <SoundInput.hpp>
#include <SoundChunk.hpp>

namespace flo {

    class SingleSoundInput : public Singular<SoundInput, EmptySoundState> {
    public:

        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state);
    };

} // namespace flo
