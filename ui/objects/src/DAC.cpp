#include <Flosion/UI/Objects/DAC.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/Font.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    DAC::DAC(){
        addToInflow(makePeg(&m_dac.soundResult.getInput(), "Input"));
        addToInflow(makePeg(&m_dac.soundResult.getInput().currentTime, "Current Time"));


        auto bod = std::make_unique<ui::Boxed<ui::VerticalList>>();
        bod->push_back<ui::Text>("DAC", getFont());
        bod->push_back<ui::ToggleButton>(
            false,
            getFont(),
            [&](bool v){
                if (v){
                    m_dac.play();
                } else {
                    m_dac.pause();
                }
            },
            std::pair<ui::String, ui::String>{"Play", "Pause"}
        );
        bod->push_back<ui::CallbackButton>(
            "Reset",
            getFont(),
            [&](){
                const auto s = m_dac.getStatus();
                m_dac.stop();
                m_dac.soundResult.reset();
                if (s == sf::SoundStream::Playing){
                    m_dac.play();
                }
            }
        );
        bod->setPadding(5.0f);
        bod->setBackgroundColor(0x22BB22FF);
        setBody(std::move(bod));
    }

    void DAC::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void DAC::deserialize(Deserializer& d){
        deserializePegs(d);
    }

    RegisterFactoryObject(DAC, "DAC");

} // namespace flui
