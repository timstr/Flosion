#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Resampler.hpp>

namespace flui {

    class Resampler : public SoundObject {
    public:
        Resampler();

    private:
        flo::Resampler m_resampler;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
