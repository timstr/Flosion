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
        SoundInputPeg(Object* parentObject, flo::SoundInput* input, ui::String label);

        // TODO
    };

    class SoundOutputPeg : public OutputPeg<SoundWireTraits> {
    public:
        SoundOutputPeg(Object* parentObject, flo::SoundSource* output, ui::String label);

        // TODO
    };

    class SoundWireHead : public WireHead<SoundWireTraits> {
    public:
        SoundWireHead(SoundWire*);

        // TODO
    };

    class SoundWireTail : public WireTail<SoundWireTraits> {
    public:
        SoundWireTail(SoundWire*);

        // TODO
    };

    class SoundWire : public Wire<SoundWireTraits> {
    public:
        using Wire::Wire;

        // TODO

    private:
        void render(sf::RenderWindow&) override;
    };

} // namespace flui
