#include <SoundSource.hpp>

namespace flo {

    SoundNumberInput::SoundNumberInput(SoundNode* owner, double defaultValue) noexcept
        : NumberInput(defaultValue) {
        setStateOwner(owner);
        // NOTE: this is done here to prevent a call to a yet-unimplemented
        // virtual function (getStateOwner() in canAddDependency())
        addDependency(&m_constant);
    }

} // namespace flo
