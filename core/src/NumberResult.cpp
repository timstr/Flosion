#include <NumberResult.hpp>

namespace flo {

    NumberResult::NumberResult() noexcept 
        : m_hiddenNode(std::make_unique<SoundResult>())
        , input(m_hiddenNode.get()) {
        setStateOwner(m_hiddenNode.get());
    }

    double NumberResult::getValue() const noexcept {
        return input.getValue(m_hiddenNode->getMonoState());
    }

} // namespace flo
