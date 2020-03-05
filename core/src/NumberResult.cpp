#include <Flosion/Core/NumberResult.hpp>

namespace flo {

    NumberResult::NumberResult() noexcept 
        : m_hiddenNode(std::make_unique<SoundResult>())
        , input(m_hiddenNode.get()) {
        setStateOwner(m_hiddenNode.get());
    }

    double NumberResult::getValue() const noexcept {
        const auto& m_hiddenNodeButConst = m_hiddenNode; // Grumble grumble grumble...
        return input.getValue(m_hiddenNodeButConst->getMonoState());
    }

} // namespace flo
