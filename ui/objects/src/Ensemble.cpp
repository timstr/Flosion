#include <Flosion/UI/Objects/Ensemble.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Ensemble::Ensemble()
        : SoundObject(&m_ensemble) {
        addToInflow(makePeg(&m_ensemble.input.frequencyOut, "Frequency Out"));
        addToInflow(makePeg(&m_ensemble.input));
        addToInflow(makePeg(&m_ensemble.frequencyIn, "Frequency In"));
        addToInflow(makePeg(&m_ensemble.frequencySpread, "Spread Factor"));
        addToOutflow(makePeg(&m_ensemble));
        setBody(makeSimpleBody("Ensemble", 0x99BB22FF));
    }

    void Ensemble::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void Ensemble::deserialize(Deserializer& d){
        deserializePegs(d);
    }

    RegisterFactoryObject(Ensemble, "Ensemble");
    REGISTER_SERIALIZABLE(Ensemble, "Ensemble");

} // namespace flui
