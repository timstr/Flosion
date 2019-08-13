#pragma once

#include <State.hpp>
#include <cstdint>

namespace flo {

    class SoundNode;

    class SoundState : public State {
    public:
        SoundState(SoundNode* owner, const SoundState* dependentState) noexcept;

        const SoundState* getDependentState() const noexcept;

        SoundNode* getOwner() noexcept;
        const SoundNode* getOwner() const noexcept;

        double getElapsedTimeAt(const SoundNode* node) const noexcept;
        
        void adjustTime(std::uint32_t offset);
        
    private:
        SoundNode* m_owner;
        const SoundState* m_dependentState;
        std::uint32_t m_coarseTime;
        std::uint32_t m_fineTime;

        friend class StateTable;

        template<typename SoundStateType>
        friend class ControlledSoundSource;

        template<typename SoundStateType>
        friend class UncontrolledSoundSource;
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
