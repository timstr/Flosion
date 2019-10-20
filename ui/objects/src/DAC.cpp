#include <Flosion/UI/Objects/DAC.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    DAC::DAC(){
        addSoundInput(&m_dac.soundResult.getInput(), "Input");
        auto bod = std::make_unique<ui::Boxed<ui::GridContainer>>(1, 3);
        bod->putCell<ui::Text>(0, 0, "DAC", getFont());
        bod->putCell<ui::ToggleButton>(
            0,
            1,
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
        bod->putCell<ui::CallbackButton>(
            0,
            2,
            "Reset",
            getFont(),
            [&](){
                m_dac.soundResult.reset();
            }
        );
        setBody(std::move(bod));
    }
    RegisterFactoryObject(DAC, "DAC");

} // namespace flui
