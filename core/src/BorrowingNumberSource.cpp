#include <Flosion/Core/BorrowingNumberSource.hpp>
#include <Flosion/Core/SoundSource.hpp>

namespace flo {

    BorrowingNumberSource::BorrowingNumberSource() noexcept
        : m_stateLender(nullptr)
        , m_stateOffset(static_cast<std::size_t>(-1)) {

    }

    BorrowingNumberSource::~BorrowingNumberSource(){
        borrowFrom(nullptr);
    }

    SoundSource* BorrowingNumberSource::getStateLender() noexcept {
        return m_stateLender;
    }

    const SoundSource* BorrowingNumberSource::getStateLender() const noexcept {
        return m_stateLender;
    }

    void BorrowingNumberSource::borrowFrom(SoundSource* node){
        const auto prev = m_stateLender;
        if (m_stateLender){
            m_stateLender->removeBorrower(this);
            auto it = std::find(m_stateLender->m_borrowers.begin(), m_stateLender->m_borrowers.end(), this);
            assert(it != m_stateLender->m_borrowers.end());
            m_stateLender->m_borrowers.erase(it);
        }
        m_stateLender = node;
        if (m_stateLender){
            m_stateLender->addBorrower(this);
            assert(std::count(m_stateLender->m_borrowers.begin(), m_stateLender->m_borrowers.end(), this) == 0);
            m_stateLender->m_borrowers.push_back(this);
        }
        setStateOwner(m_stateLender);

        if (prev){
            onStopBorrowing.broadcast(prev);
            prev->onBorrowerRemoved.broadcast(this);
        }
        if (m_stateLender){
            onStartBorrowing.broadcast(m_stateLender);
            m_stateLender->onBorrowerAdded.broadcast(this);
        }
    }

} // namespace flo 
