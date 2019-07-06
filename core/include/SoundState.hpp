#pragma once

#include <State.hpp>

namespace flo::sound {

    class StateOwner;

    class SoundState : public State {
    public:
        SoundState(StateOwner* owner, const SoundState* dependentState) noexcept;

        const SoundState* getDependentState() const noexcept;

        StateOwner& getOwner() noexcept;
        const StateOwner& getOwner() const noexcept;
        
    private:
        StateOwner* m_owner;
        const SoundState* m_dependentState;
    };

} // namespace flo::sound
