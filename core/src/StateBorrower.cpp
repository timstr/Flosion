#include <StateBorrower.hpp>

#include <SoundNode.hpp>

namespace flo {

    StateBorrower::StateBorrower() noexcept
        : m_stateLender(nullptr)
        , m_stateOffset(-1) {

    }

    SoundNode* StateBorrower::getStateLender() noexcept {
        return m_stateLender;
    }

    const SoundNode* StateBorrower::getStateLender() const noexcept {
        return m_stateLender;
    }

    void StateBorrower::borrowFrom(SoundNode* node){
        if (m_stateLender){
            m_stateLender->removeBorrower(this);
        }
        m_stateLender = node;
        if (m_stateLender){
            m_stateLender->addBorrower(this);
        }
    }

} // namespace flo
