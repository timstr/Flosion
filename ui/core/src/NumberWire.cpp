#include <Flosion/UI/Core/NumberWire.hpp>

#include <Flosion/UI/Core/Panel.hpp>
#include <Flosion/UI/Core/NumberPegs.hpp>

#include <array>

namespace flui {

    NumberInputPeg::NumberInputPeg(Object* parentObject, flo::NumberInput* input, ui::String label)
        : InputPeg(parentObject, input, std::move(label)) {

        setBackgroundColor(0x4f7bffff);
    }

    NumberOutputPeg::NumberOutputPeg(Object* parentObject, flo::NumberSource* output, ui::String label)
        : OutputPeg(parentObject, output, std::move(label)){

        setBackgroundColor(0x13139eff);
    }

    NumberWireHead::NumberWireHead(NumberWire* w)
        : WireHead(w) {

        setBackgroundColor(0x0080FFFF);
    }

    NumberWireTail::NumberWireTail(NumberWire* w)
        : WireTail(w) {

        setBackgroundColor(0x0055AAFF);
    }

    void NumberWire::render(sf::RenderWindow& rw){
        ui::FreeContainer::render(rw);
        auto h = getHead();
        auto t = getTail();
        const auto pts = std::array<sf::Vertex, 2>{
            sf::Vertex{t->pos() + t->size() * 0.5f, sf::Color{0x000088FF}},
            sf::Vertex{h->pos() + h->size() * 0.5f, sf::Color{0x0000FFFF}}
        };
        rw.draw(pts.data(), pts.size(), sf::PrimitiveType::Lines);
    }

} // namespace flui
