#include <Flosion/UI/Objects/WaveGenerator.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    WaveGenerator::WaveGenerator()
        : SoundObject(&m_waveGen) {
        addToInflow(makePeg(&m_waveGen.phase, "Phase"));
        addToInflow(makePeg(&m_waveGen.waveFunction, "Wave Function"));
        addToInflow(makePeg(&m_waveGen.frequency, "Frequency"));
        addToInflow(makePeg(&m_waveGen.currentTime, "Current Time"));
        addToOutflow(makePeg(&m_waveGen, "Output"));

        auto fc = std::make_unique<ui::Boxed<ui::FreeContainer>>();
        fc->setBackgroundColor(0x22BB22FF);
        auto& bod = fc->add<ui::VerticalList>(ui::FreeContainer::Center, ui::FreeContainer::Center);
        bod.push_back<ui::Text>("Wave Generator", getFont());
        bod.push_back<ui::ToggleButton>(
            false,
            getFont(),
            [&](bool v){
                m_waveGen.setPhaseSync(v);
            },
            std::pair<ui::String, ui::String>{"Sync Off", "Sync On"}
        );
        bod.setPadding(5.0f);
        setBody(std::move(fc));
    }

    void WaveGenerator::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void WaveGenerator::deserialize(Deserializer& d) {
        deserializePegs(d);
    }

    RegisterFactoryObject(WaveGenerator, "WaveGenerator");

} // namespace flui
