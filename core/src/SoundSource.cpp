#include <Flosion/Core/SoundSource.hpp>

namespace flo {

    SoundNumberInput::SoundNumberInput(SoundNode* owner, double defaultValue) noexcept
        : NumberInput(defaultValue) {
        setStateOwner(owner);
    }

    SoundSource::~SoundSource(){
        // Disconnect all number sources and inputs
        // TODO: how to do this without dynamic_cast?
        for (auto n : getNumberNodes()){
            if (auto ns = dynamic_cast<NumberSource*>(n)){
                while (ns->getInputs().size() > 0){
                    ns->getInputs().front()->setSource(nullptr);
                }
            } else if (auto ni = dynamic_cast<NumberInput*>(n)){
                ni->setSource(nullptr);
            }
        }
    }

    SoundInput::~SoundInput(){
        for (auto n : getNumberNodes()){
            if (auto ns = dynamic_cast<NumberSource*>(n)){
                while (ns->getInputs().size() > 0){
                    ns->getInputs().front()->setSource(nullptr);
                }
            } else if (auto ni = dynamic_cast<NumberInput*>(n)){
                ni->setSource(nullptr);
            }
        }
    }

} // namespace flo
