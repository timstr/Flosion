#include <SoundState.hpp>

namespace flo {

    SoundState::SoundState(SoundNode* owner, const SoundState* dependentState) noexcept
        : m_owner(owner)
        , m_dependentState(dependentState) {
    
    }

    void EmptySoundState::reset() noexcept {

    }

} // namespace flo
