#include <Flosion/UI/Objects/DAC.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/Font.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    DAC::DAC(){
        addToLeft(makePeg(&m_dac.soundResult.getInput(), "Input"));
        addToTop(makePeg(&m_dac.soundResult.getInput().currentTime, "Current Time"));


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
                m_dac.soundResult.reset();
            }
        );
        bod->setPadding(5.0f);
        bod->setBackgroundColor(0x22BB22FF);
        setBody(std::move(bod));
    }
    RegisterFactoryObject(DAC, "DAC");

} // namespace flui
