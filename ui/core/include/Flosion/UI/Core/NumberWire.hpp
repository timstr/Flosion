#pragma once

#include <Flosion/Core/NumberSource.hpp>
#include <Flosion/UI/Core/WireBase.hpp>

namespace flui {

    class NumberInputPeg;
    class NumberOutputPeg;
    class NumberWire;
    class NumberWireHead;
    class NumberWireTail;

    struct NumberWireTraits {
        using OutputType = flo::NumberSource;
        using InputType = flo::NumberInput;
        using InputPegType = NumberInputPeg;
        using OutputPegType = NumberOutputPeg;
        using WireType = NumberWire;
        using WireHeadType = NumberWireHead;
        using WireTailType = NumberWireTail;
    };

    class NumberInputPeg : public InputPeg<NumberWireTraits> {
    public:
        using InputPeg::InputPeg;
        // TODO
    };

    class NumberOutputPeg : public OutputPeg<NumberWireTraits> {
    public:
        using OutputPeg::OutputPeg;
        // TODO
    };

    class NumberWireHead : public WireHead<NumberWireTraits> {
    public:
        using WireHead::WireHead;
        // TODO
    };

    class NumberWireTail : public WireTail<NumberWireTraits> {
    public:
        using WireTail::WireTail;
        // TODO
    };

    class NumberWire : public Wire<NumberWireTraits> {
    public:
        using Wire::Wire;
        // TODO
    };

} // namespace flui
