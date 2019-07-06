#pragma once

#include <SoundInput.hpp>
#include <KeyedStateMap.hpp>

namespace flo::sound {

    template<typename StateType, typename KeyType>
    class MultiSoundInput : public SoundInput, public KeyStateMap<StateType, KeyType> {
        // TODO
    };

} // namespace flo::sound
