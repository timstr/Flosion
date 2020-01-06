#pragma once

#include <Flosion/Core/NumberSource.hpp>

namespace flo {

    class SoundSource;

    class BorrowingNumberSource : public NumberSource {
    public:
        BorrowingNumberSource() noexcept;
        ~BorrowingNumberSource();

        virtual std::unique_ptr<StateAllocator> makeAllocater() const = 0;

        SoundSource* getStateLender() noexcept;
        const SoundSource* getStateLender() const noexcept;

        void borrowFrom(SoundSource*);

        Signal<const SoundSource*> onStartBorrowing;
        Signal<const SoundSource*> onStopBorrowing;

    private:
        SoundSource* m_stateLender;
        size_t m_stateOffset;

        friend class StateTable;
    };

    template<typename StateType>
    class BorrowingNumberSourceTemplate : public BorrowingNumberSource {
    public:

        virtual double evaluate(StateType* state, const SoundState* context) const noexcept = 0;

    private:
        double evaluate(const SoundState* context) const noexcept override final;

        std::unique_ptr<StateAllocator> makeAllocater() const override final;
    };

} // namespace flo

#include <Flosion/Core/BorrowingNumberSource.tpp>
