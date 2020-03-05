#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Ensemble.hpp>

namespace flui {

    class Ensemble : public SoundObject {
    public:
        Ensemble();

    private:
        flo::Ensemble m_ensemble;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
