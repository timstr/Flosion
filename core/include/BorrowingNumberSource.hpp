#include <NumberSource.hpp>

namespace flo {

    class BorrowingNumberSource : public NumberSource {
    public:
        BorrowingNumberSource() noexcept;
        ~BorrowingNumberSource();

        virtual std::unique_ptr<StateAllocator> makeAllocater() const = 0;

        SoundNode* getStateLender() noexcept;
        const SoundNode* getStateLender() const noexcept;

        void borrowFrom(SoundNode*);

    private:
        SoundNode* m_stateLender;
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

#include <BorrowingNumberSource.tpp>
