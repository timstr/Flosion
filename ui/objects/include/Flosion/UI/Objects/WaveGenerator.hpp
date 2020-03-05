#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/WaveGenerator.hpp>

namespace flui {

    class WaveGenerator : public SoundObject {
    public:
        WaveGenerator();

    private:
        flo::WithCurrentTime<flo::WaveGenerator> m_waveGen;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
