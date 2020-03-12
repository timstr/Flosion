#include <Flosion/UI/Objects/Resampler.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Resampler::Resampler()
        : SoundObject(&m_resampler) {
        addToInflow(makePeg(&m_resampler.currentTime, "Current Time"));
        addToInflow(makePeg(&m_resampler.timeSpeed, "Time Speed"));
        addToInflow(makePeg(&m_resampler.input));
        addToOutflow(makePeg(&m_resampler));
        setBody(makeSimpleBody("Resampler", 0xd47b00ff));
    }

    void Resampler::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void Resampler::deserialize(Deserializer& d) {
        deserializePegs(d);
    }

    RegisterFactoryObject(Resampler, "Resampler");
    REGISTER_SERIALIZABLE(Resampler, "Resampler")

} // namespace flui
