#include <SoundState.hpp>

#include <Sample.hpp>
#include <SoundNode.hpp>

#include <cassert>

namespace flo {

    SoundState::SoundState(SoundNode* owner, const SoundState* dependentState) noexcept
        : m_owner(owner)
        , m_dependentState(dependentState)
        , m_coarseTime(0)
        , m_fineTime(0) {
    
    }

    const SoundState* SoundState::getDependentState() const noexcept {
        return m_dependentState;
    }

    SoundNode* SoundState::getOwner() noexcept {
        return m_owner;
    }

    const SoundNode* SoundState::getOwner() const noexcept {
        return m_owner;
    }

    double SoundState::getElapsedTimeAt(const SoundNode* node) const noexcept {
        auto curr = this;
        auto base = static_cast<double>(m_coarseTime);
        auto offset = static_cast<double>(m_fineTime);
        while (curr){
            auto owner = curr->getOwner();
            if (owner == node){
                const auto elapsed = (base + offset) / static_cast<double>(Sample::frequency);
                return elapsed;
            }
            offset *= owner->getTimeSpeed(curr);
            curr = curr->getDependentState();
        }
        assert(false);
        return 0.0;
    }

    void SoundState::adjustTime(std::uint32_t offset){
        m_fineTime = offset;
    }

    void EmptySoundState::reset() noexcept {

    }

} // namespace flo
