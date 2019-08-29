#include <BorrowingNumberSource.hpp>
#include <SoundNode.hpp>

namespace flo {

    BorrowingNumberSource::BorrowingNumberSource() noexcept
        : m_stateLender(nullptr)
        , m_stateOffset(-1) {

    }

    BorrowingNumberSource::~BorrowingNumberSource(){
        borrowFrom(nullptr);
    }

    SoundNode* BorrowingNumberSource::getStateLender() noexcept {
        return m_stateLender;
    }

    const SoundNode* BorrowingNumberSource::getStateLender() const noexcept {
        return m_stateLender;
    }

    void BorrowingNumberSource::borrowFrom(SoundNode* node){
        if (m_stateLender){
            m_stateLender->removeBorrower(this);
        }
        m_stateLender = node;
        if (m_stateLender){
            m_stateLender->addBorrower(this);
        }
        setStateOwner(m_stateLender);
    }

} // namespace flo 
