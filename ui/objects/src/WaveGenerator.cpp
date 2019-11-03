#include <Flosion/UI/Objects/WaveGenerator.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    WaveGenerator::WaveGenerator(){
        addToTop(makeNumberOutput(&m_waveGen.phase, "Phase"));
        addToLeft(makeNumberInput(&m_waveGen.waveFunction, "Wave Function"));
        addToLeft(makeNumberInput(&m_waveGen.frequency, "Frequency"));
        addtoRight(makeSoundOutput(&m_waveGen, "Output"));

        auto bod = std::make_unique<ui::Boxed<ui::GridContainer>>(1, 2);
        bod->putCell<ui::Text>(0, 0, "Wave Generator", getFont());
        bod->putCell<ui::ToggleButton>(
            0,
            1,
            false,
            getFont(),
            [&](bool v){
                m_waveGen.setPhaseSync(v);
            },
            std::pair<ui::String, ui::String>{"Sync Off", "Sync On"}
        );
        setBody(std::move(bod));
    }
    RegisterFactoryObject(WaveGenerator, "WaveGenerator");

} // namespace flui
