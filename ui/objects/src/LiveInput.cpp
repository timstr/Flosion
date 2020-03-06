#include <Flosion/UI/Objects/LiveInput.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

#include <Flosion/UI/Core/Font.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    LiveInput::LiveInput()
        : SoundObject(&m_liveInput) {

        addToOutflow(makePeg(&m_liveInput));

        auto bod = std::make_unique<ui::Boxed<ui::VerticalList>>();
        bod->setPadding(10.0f);
        bod->setBackgroundColor(0xDED300FF);

        m_label = &bod->push_back<ui::Text>("LiveInput", getFont());

        bod->push_back<ui::CallbackButton>(
            "Start",
            getFont(),
            [&](){
                m_liveInput.start();
            }
        );
        bod->push_back<ui::CallbackButton>(
            "Stop",
            getFont(),
            [&](){
                m_liveInput.stop();
            }
        );

        m_deviceCont = &bod->push_back<ui::FreeContainer>();

        showDeviceButton();

        setBody(std::move(bod));
    }

    void LiveInput::showDeviceButton(){
        assert(m_deviceCont);
        m_deviceCont->clear();
        m_deviceCont->add<ui::CallbackButton>(
            ui::FreeContainer::InsideLeft,
            ui::FreeContainer::InsideLeft,
            "Device",
            getFont(),
            [this](){
                showDeviceList();
            }
        );
    }

    void LiveInput::showDeviceList(){
        assert(m_deviceCont);
        m_deviceCont->clear();
        auto& l = m_deviceCont->add<ui::VerticalList>(
            //ui::FreeContainer::InsideLeft,
            //ui::FreeContainer::InsideLeft
        );

        for (const auto& d : m_liveInput.getAvailableDevices()) {
            l.push_back<ui::CallbackButton>(
                d,
                getFont(),
                [this, devName = d](){
                    m_liveInput.selectDevice(devName);
                    showDeviceButton();
                }
            );
        }
    }

    void LiveInput::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void LiveInput::deserialize(Deserializer& d){
        deserializePegs(d);
    }

    RegisterFactoryObject(LiveInput, "LiveInput", "Microphone");
    REGISTER_SERIALIZABLE(LiveInput, "LiveInput");

} // namespace flui
