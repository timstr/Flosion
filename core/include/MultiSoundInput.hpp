#pragma once

#include <SoundInput.hpp>
#include <StateTable.hpp>
#include <SoundChunk.hpp>

#include <map>

namespace flo {

    template<typename StateType, typename KeyType>
    class MultiSoundInput final : public SoundInput {
    public:
        MultiSoundInput(SoundNetwork*);

        void addKey(KeyType);
        void removeKey(KeyType);
        bool hasKey(const KeyType&) const noexcept;

        void resetStateFor(const SoundNode* node, const SoundState* state, const KeyType& key);

        StateType& getStateFor(const SoundNode* node, const SoundState* state, const KeyType& key) noexcept;

        void getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state, const KeyType& key);

    private:
        void addStateFor(const SoundNode* node, const SoundState* state) override final;
        void removeStateFor(const SoundNode* node, const SoundState* state) override final;
        void resetStateFor(const SoundNode* node, const SoundState* state) override final;

        void onStateMoved(const SoundNode* node, const SoundState* from, const SoundState* to) override final;

        size_t getMultiplicity() const noexcept override final;

        std::map<KeyType, size_t> m_keyOffsets;
        std::map<SoundNode*, size_t> m_dependentOffsets;

        StateTable<StateType> m_stateArray;
    };

} // namespace flo
