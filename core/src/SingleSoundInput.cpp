#include <SingleSoundInput.hpp>

namespace flo {

    SingleSoundInput::SingleSoundInput()
        : SoundInput(Propagation::Singular, std::make_unique<ConcreteStateAllocator<EmptySoundState>>()) {
    
    }

} // namespace flo
