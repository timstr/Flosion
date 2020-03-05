#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/AudioClip.hpp>

namespace flui {

    class AudioClip : public SoundObject {
    public:
        AudioClip();

    private:
        void loadFromFile(const std::string&);

        ui::Text* m_label;
        flo::AudioClip m_audioClip;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
