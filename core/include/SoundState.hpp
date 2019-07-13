#pragma once

#include <State.hpp>

namespace flo {

    class SoundNode;

    // TODO: does every SoundState belong to a StateTable?
    // If so, consider making owner a StateTable* instead

    class SoundState : public State {
    public:
        SoundState(SoundNode* owner, const SoundState* dependentState) noexcept;

        const SoundState* getDependentState() const noexcept;

        SoundNode& getOwner() noexcept;
        const SoundNode& getOwner() const noexcept;
        
    private:
        SoundNode* m_owner;
        const SoundState* m_dependentState;
    };

} // namespace flo
