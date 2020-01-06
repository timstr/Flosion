#pragma once

#include <Flosion/UI/Core/Object.hpp>

namespace flui {

    class BorrowingNumberObject;

    class SoundObject : public Object {
    public:
        SoundObject(flo::SoundSource*);
        ~SoundObject();

        void attachBorrower(BorrowingNumberObject*);
        void detachBorrower(BorrowingNumberObject*);
        
        flo::SoundSource* getSoundSource() noexcept;
        const flo::SoundSource* getSoundSource() const noexcept;

    private:
        const SoundObject* toSoundObject() const noexcept override final;

        bool onDrop(ui::Draggable*) override;

        void updateLayout(FlowDirection) override;

        void initialize() override;

        std::vector<BorrowingNumberObject*> m_borrowers;
        ListVariant m_borrowerList;

        flo::SoundSource* const m_soundSource;

        flo::Connection m_onBorrowerAddedConn;
        flo::Connection m_onBorrowerRemovedConn;

        friend Panel;
    };
    

} // namespace flui
