#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Mixer.hpp>

namespace flui {

    class Mixer : public SoundObject {
    public:
        Mixer();

    private:
        bool onDrop(ui::Draggable*) override;

        flo::Mixer m_mixer;
        std::map<const SoundInputPeg*, flo::Connection> m_connections;
    };

} // namespace flui
