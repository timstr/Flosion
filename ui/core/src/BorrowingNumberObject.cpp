#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/Core/BorrowingNumberSource.hpp>
#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/UI/Core/Panel.hpp>

namespace flui {

    BorrowingNumberObject::BorrowingNumberObject(flo::BorrowingNumberSource* bns)
        : m_borrower(bns)
        , m_lender(nullptr) {
    
    }

    BorrowingNumberObject::~BorrowingNumberObject(){
        // NOTE: m_borrower is expected to be a dangling pointer at this point
    }

    flo::BorrowingNumberSource* BorrowingNumberObject::getBorrower() noexcept {
        return m_borrower;
    }

    const flo::BorrowingNumberSource* BorrowingNumberObject::getBorrower() const noexcept {
        return m_borrower;
    }

    const BorrowingNumberObject* BorrowingNumberObject::toBorrowingNumberObject() const noexcept {
        return this;
    }

    bool BorrowingNumberObject::onLeftClick(int){
        // TODO: highlight sound objects
        startDrag();
        if (m_borrower->getStateLender()){
            assert(m_lender);
            assert(m_borrower->getStateLender() == m_lender->getSoundSource());
            m_borrower->borrowFrom(nullptr);
            transferEventResposeTo(this);
        }
        return true;
    }

    void BorrowingNumberObject::onLeftRelease(){
        assert(m_lender == nullptr);
        stopDrag();
        drop(localMousePos());
    }

    Object::FlowDirection BorrowingNumberObject::getNewFlowDirection(FlowDirection desired) const {
        if (!m_lender){
            return desired;
        }
        const auto lenderDir = m_lender->getFlowDirection();
        const auto lenderIsVertical = lenderDir == FlowDirection::Up || lenderDir == FlowDirection::Down;
        const auto wouldBeVertical = desired == FlowDirection::Up || desired == FlowDirection::Down;
        return lenderIsVertical == wouldBeVertical ? desired : lenderDir;
    }

} // namespace flui 
