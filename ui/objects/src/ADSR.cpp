#include <Flosion/UI/Objects/ADSR.hpp>

#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    ADSR::ADSR() {
        addToInflow(makePeg(&m_adsr.currentTime, "Current Time"));
        addToInflow(makePeg(&m_adsr.attackDuration, "Attack Duration"));
        addToInflow(makePeg(&m_adsr.decayDuration, "Decay Duration"));
        addToInflow(makePeg(&m_adsr.totalDuration, "Total Duration"));
        addToInflow(makePeg(&m_adsr.timeOfRelease, "Time Of Release"));
        addToInflow(makePeg(&m_adsr.sustainLevel, "Sustain Level"));

        addToOutflow(makePeg(&m_adsr, "Output"));
        setBody(makeSimpleBody("ADSR", 0xA3D9B6FF));
    }

    void ADSR::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void ADSR::deserialize(Deserializer& d){
        deserializePegs(d);
    }

    RegisterFactoryObject(ADSR, "ADSR");

} // namespace flui
