#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/DAC.hpp>

namespace flui {

    // TODO: should DAC be a SoundObject?
    // Does it make sense to borrow state from a DAC?
    class DAC : public Object {
    public:
        DAC();

    private:
        flo::DAC m_dac;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
