#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/SoundInput.hpp>

namespace flo {

    SoundNumberInput::SoundNumberInput(SoundNode* owner, double defaultValue) noexcept
        : NumberInput(defaultValue) {
        setStateOwner(owner);
    }

    SoundSource::~SoundSource(){
        notifyReactors(&SoundSourceReactor::onDestroySoundSource);
        while (m_inputs.size() > 0){
            assert(m_inputs.back());
            assert(m_inputs.back()->getSource() == this);
            m_inputs.back()->setSource(nullptr);
        }
    }

} // namespace flo
