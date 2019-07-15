#include <SoundInput.hpp>

namespace flo {

    SoundInput::SoundInput(Propagation statePropagation, std::unique_ptr<StateAllocator> allocator)
        : IOSoundNode(Controllability::Controllable, statePropagation, TimeSync::Realtime, std::move(allocator)){

    }

    void SoundInput::setSource(SoundSource* src){
        // TODO: remove all states from old source, add all states to new source
    }

} // namespace flo
