#pragma once

#include <Flosion/Core/SoundInput.hpp>
#include <Flosion/Core/SoundChunk.hpp>

namespace flo {

    class SingleSoundInput : public Singular<SoundInput, EmptySoundState> {
    public:

        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state);
    };

} // namespace flo
