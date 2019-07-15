#include <SoundSource.hpp>

namespace flo {

    SoundSource::SoundSource(Controllability controllability, TimeSync timeSync, std::unique_ptr<StateAllocator> allocator)
        : IOSoundNode(controllability, Propagation::Singular, timeSync, std::move(allocator)) {
    
    }

} // namespace flo
