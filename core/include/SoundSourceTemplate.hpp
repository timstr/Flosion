#pragma once

#include <SoundSource.hpp>
#include <StateTable.hpp>

#include <map>

namespace flo {

    class SoundInput;

    template<typename SoundStateType>
    class SoundSourceTemplate : public SoundSource, public StateTable {
    public:
        SoundSourceTemplate(SoundNetwork* network, Type type);

        virtual void renderNextChunk(SoundChunk& chunk, SoundStateType* state) = 0;

    private:
        void getNextChunkFor(SoundChunk&, const SoundState*) override final;

        void addStateFor(const SoundNode* node, const SoundState* state) override final;
        void removeStateFor(const SoundNode* node, const SoundState* state) override final;
        void resetStateFor(const SoundNode* node, const SoundState* state) override final;

        void onStateMoved(const SoundNode* node, const SoundState* from, const SoundState* to) override final;

        std::map<SoundNode*, size_t> m_dependentOffsets;
    };

} // namespace flo

#include <SoundSourceTemplate.tpp>
