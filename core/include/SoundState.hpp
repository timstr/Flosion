#pragma once

#include <State.hpp>

namespace flo {

    class ISoundNode;

    class SoundState : public State {
    public:
        SoundState(ISoundNode* owner, const SoundState* dependentState) noexcept;

        const SoundState* getDependentState() const noexcept;

        ISoundNode& getOwner() noexcept;
        const ISoundNode& getOwner() const noexcept;
        
    private:
        ISoundNode* m_owner;
        const SoundState* m_dependentState;
    };

    class EmptySoundState final : public SoundState {
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
