#pragma once

#include <Flosion/UI/Core/NumberObject.hpp>

#include <Flosion/Core/BorrowingNumberSource.hpp>

namespace flui {

    class SoundObject;

    // Number pegs only
    // Pegs on left/right sides only

    // TODO: when a BorrowingNumberObject is attached to a SoundObject,
    // restrict its flow directions to be with or against the flow
    // direction of the SoundObject

    class BorrowingNumberObject : public NumberObject {
    public:
        BorrowingNumberObject(flo::BorrowingNumberSource*);
        ~BorrowingNumberObject();

        flo::BorrowingNumberSource* getBorrower() noexcept;
        const flo::BorrowingNumberSource* getBorrower() const noexcept;

    private:
        const BorrowingNumberObject* toBorrowingNumberObject() const noexcept override final;

        bool onLeftClick(int) override;

        void onLeftRelease() override;

        FlowDirection getNewFlowDirection(FlowDirection) const override;

        flo::BorrowingNumberSource* const m_borrower;
        SoundObject* m_lender;

        friend class SoundObject;
    };

} // namespace flui
