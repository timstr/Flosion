#pragma once

#include <State.hpp>

namespace flo {

    class SoundNode;

    class SoundState : public State {
    public:
        SoundState(SoundNode* owner, const SoundState* dependentState) noexcept;

        const SoundState* getDependentState() const noexcept;

        SoundNode& getOwner() noexcept;
        const SoundNode& getOwner() const noexcept;
        
    private:
        SoundNode* m_owner;
        const SoundState* m_dependentState;

        friend class StateTable;
    };

    class EmptySoundState final : public SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override final;
    };

    template<typename SoundSourceType>
    class ConcreteSoundState : public SoundState {
    public:
        using SoundState::SoundState;

        SoundSourceType& getOwner() noexcept;
        const SoundSourceType& getOwner() const noexcept;
    };

} // namespace flo
