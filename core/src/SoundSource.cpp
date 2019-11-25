#include <Flosion/Core/SoundSource.hpp>

namespace flo {

    SoundNumberInput::SoundNumberInput(SoundNode* owner, double defaultValue) noexcept
        : NumberInput(defaultValue) {
        setStateOwner(owner);
    }

    SoundSource::~SoundSource(){
        for (auto n : getNumberNodes()){
            if (auto ns = n->toNumberSource()){
                ns->disconnectAllInputs();
            } else if (auto ni = n->toNumberInput()){
                ni->setSource(nullptr);
            }
        }
    }

    SoundInput::~SoundInput(){
        for (auto n : getNumberNodes()){
            if (auto ns = n->toNumberSource()){
                ns->disconnectAllInputs();
            } else if (auto ni = n->toNumberInput()){
                ni->setSource(nullptr);
            }
        }
    }

} // namespace flo
