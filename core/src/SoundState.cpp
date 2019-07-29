#include <SoundState.hpp>

namespace flo {

    SoundState::SoundState(SoundNode* owner, const SoundState* dependentState) noexcept
        : m_owner(owner)
        , m_dependentState(dependentState) {
    
    }

    const SoundState* SoundState::getDependentState() const noexcept {
        return m_dependentState;
    }

    SoundNode* SoundState::getOwner() noexcept {
        return m_owner;
    }

    const SoundNode * SoundState::getOwner() const noexcept {
        return m_owner;
    }

    void EmptySoundState::reset() noexcept {

    }

} // namespace flo
