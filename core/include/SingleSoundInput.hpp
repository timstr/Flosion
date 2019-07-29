#pragma once

#include <SoundInput.hpp>
#include <SoundChunk.hpp>

namespace flo {

    class SingleSoundInput final : public Singular<SoundInput, EmptySoundState> {
    public:

        // TODO: is 'node' needed here?
        // Classically, the node is always the SoundSource which owns
        // the SoundInput.
        // It *may* be needed for the Router object.
        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state);
    };

} // namespace flo
