#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/AudioClip.hpp>

namespace flui {

    class AudioClip : public Object {
    public:
        AudioClip();

    private:
        void loadFromFile(const std::string&);

        ui::Text* m_label;
        flo::AudioClip m_audioClip;
    };

} // namespace flui
