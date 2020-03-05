#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Mixer.hpp>

namespace flui {

    class Mixer : public SoundObject {
    public:
        Mixer();

    private:
        bool onDrop(ui::Draggable*) override;

        flo::Mixer m_mixer;
        flo::Connection m_inputAddedConnection;
        flo::Connection m_inputRemovedConnection;
        std::map<const SoundInputPeg*, flo::Connection> m_inputConnections;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
