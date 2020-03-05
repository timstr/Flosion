#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Lowpass.hpp>

namespace flui {

    class Lowpass : public SoundObject {
    public:
        Lowpass();

    private:
        flo::WithCurrentTime<flo::Lowpass> m_lowpass;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
