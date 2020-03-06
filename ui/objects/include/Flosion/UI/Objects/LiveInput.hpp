#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/LiveInput.hpp>

namespace flui {

    class LiveInput : public SoundObject {
    public:
        LiveInput();

    private:
        ui::Text* m_label;
        flo::LiveInput m_liveInput;
        ui::FreeContainer* m_deviceCont;

        void showDeviceButton();

        void showDeviceList();

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
