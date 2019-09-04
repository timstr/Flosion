#include <SoundNode.hpp>

namespace flo {

    template<typename StateType>
    inline double BorrowingNumberSourceTemplate<StateType>::evaluate(const SoundState* context) const noexcept {
        if (auto lender = getStateLender()){
            return evaluate(reinterpret_cast<StateType*>(lender->getBorrowedState(context, this)), context);
        }
        return 0.0;
    }

    template<typename StateType>
    inline std::unique_ptr<StateAllocator> BorrowingNumberSourceTemplate<StateType>::makeAllocater() const {
        return std::make_unique<ConcreteStateAllocator<StateType>>();
    }

} // namespace flo
