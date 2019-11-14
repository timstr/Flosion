#include <Flosion/UI/Core/SoundWire.hpp>

#include <Flosion/UI/Core/Panel.hpp>
#include <Flosion/UI/Core/SoundPegs.hpp>

#include <array>

namespace flui {

    SoundInputPeg::SoundInputPeg(Object* parentObject, flo::SoundInput* input, ui::String label)
        : InputPeg(parentObject, input, std::move(label)) {

        setBackgroundColor(0x4fff61ff);
    }

    SoundOutputPeg::SoundOutputPeg(Object* parentObject, flo::SoundSource* output, ui::String label)
        : OutputPeg(parentObject, output, std::move(label)){

        setBackgroundColor(0x139e1aff);
    }

    SoundWireHead::SoundWireHead(SoundWire* w)
        : WireHead(w) {

        setBackgroundColor(0x80FF00FF);
    }

    SoundWireTail::SoundWireTail(SoundWire* w)
        : WireTail(w) {

        setBackgroundColor(0x55AA00FF);
    }

    void SoundWire::render(sf::RenderWindow& rw){
        ui::FreeContainer::render(rw);
        auto h = getHead();
        auto t = getTail();
        const auto pts = std::array<sf::Vertex, 2>{
            sf::Vertex{t->pos() + t->size() * 0.5f, sf::Color{0x008800FF}},
            sf::Vertex{h->pos() + h->size() * 0.5f, sf::Color{0x00FF00FF}}
        };
        rw.draw(pts.data(), pts.size(), sf::PrimitiveType::Lines);
    }

} // namespace flui
