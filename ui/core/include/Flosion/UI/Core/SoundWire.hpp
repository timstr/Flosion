#pragma once

#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/UI/Core/WireBase.hpp>

#include <GUI/GUI.hpp>

namespace flui {

    class SoundInputPeg;
    class SoundOutputPeg;
    class SoundWire;
    class SoundWireHead;
    class SoundWireTail;

    struct SoundWireTraits {
        using OutputType = flo::SoundSource;
        using InputType = flo::SoundInput;
        using InputPegType = SoundInputPeg;
        using OutputPegType = SoundOutputPeg;
        using WireType = SoundWire;
        using WireHeadType = SoundWireHead;
        using WireTailType = SoundWireTail;
    };

    class SoundInputPeg : public InputPeg<SoundWireTraits> {
    public:
        using InputPeg::InputPeg;
        // TODO
    };

    class SoundOutputPeg : public OutputPeg<SoundWireTraits> {
    public:
        using OutputPeg::OutputPeg;
        // TODO
    };

    class SoundWireHead : public WireHead<SoundWireTraits> {
    public:
        using WireHead::WireHead;
        // TODO
    };

    class SoundWireTail : public WireTail<SoundWireTraits> {
    public:
        using WireTail::WireTail;
        // TODO
    };

    class SoundWire : public Wire<SoundWireTraits> {
    public:
        using Wire::Wire;
        // TODO
    };

} // namespace flui
