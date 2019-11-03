#pragma once

#include <Flosion/Core/SoundInput.hpp>
#include <Flosion/Core/SoundSource.hpp>

namespace flui {

    class SoundInputPeg;
    class SoundOutputPeg;
    class SoundWire;

    struct SoundTraits {
        using InputType = flo::SoundInput;
        using OutputType = flo::SoundSource;
        using InputPegType = SoundInputPeg;
        using OutputPegType = SoundOutputPeg;
        using WireType = SoundWire;

        static void connect(InputType*, OutputType*);
        static void disconnect(InputType*, OutputType*);
        static bool isConnected(const InputType*, const OutputType*);
        static bool canConnect(const InputType*, const OutputType*);
        static bool canDisconnect(const InputType*, const OutputType*);
    };

} // namespace flui
