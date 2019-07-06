#pragma once

#include <StateArray.hpp>
#include <StateOwner.hpp>
#include <SoundState.hpp>

namespace flo::sound {

    template<typename SoundStateType, typename KeyType>
    class KeyedStateMap : public StateOwner {
    public:

        void addKey(KeyType);
        void removeKey(const KeyType&);
        bool hasKey(const KeyType&);

        void addStateFor(const SoundState* parent_state);
        void removeStateFor(const SoundState* parent_state);
        void resetStateFor(const SoundState* parent_state);

        // TODO: state lookup

        SoundStateType& getStateFor(const SoundState* parent_state, const KeyType&);
        const SoundStateType& getStateFor(const SoundState* parent_state, const KeyType&) const;

    private:
        StateArray m_states;
    };

} // namespace flo::sound
