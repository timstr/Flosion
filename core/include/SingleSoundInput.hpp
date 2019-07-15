#pragma once

#include <SoundInput.hpp>
#include <SoundChunk.hpp>

namespace flo {

    class SingleSoundInput final : public SoundInput {
    public:
        SingleSoundInput();

        // TODO: is 'node' needed here?
        // Classically, the node is always the SoundSource which owns
        // the SoundInput.
        // It *may* be needed for the Router object.
        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state);

        void addStateFor(const SoundNode* node, const SoundState* state) override final;
        void removeStateFor(const SoundNode* node, const SoundState* state) override final;
        void resetStateFor(const SoundNode* node, const SoundState* state) override final;

        void onStateMoved(const SoundNode* node, const SoundState* from, const SoundState* to) override final;
    };

} // namespace flo

#include <SoundSourceTemplate.tpp>
