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
        NumberInputPeg(Object* parentObject, flo::NumberInput* input, ui::String label);

        // TODO
    };

    class NumberOutputPeg : public OutputPeg<NumberWireTraits> {
    public:
        NumberOutputPeg(Object* parentObject, flo::NumberSource* output, ui::String label);

        // TODO
    };

    class NumberWireHead : public WireHead<NumberWireTraits> {
    public:
        NumberWireHead(NumberWire*);

        // TODO
    };

    class NumberWireTail : public WireTail<NumberWireTraits> {
    public:
        NumberWireTail(NumberWire*);

        // TODO
    };

    class NumberWire : public Wire<NumberWireTraits> {
    public:
        using Wire::Wire;

        // TODO

    private:
        void render(sf::RenderWindow&) override;
    };

} // namespace flui
