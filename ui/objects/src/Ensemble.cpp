#include <Flosion/UI/Objects/Ensemble.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Ensemble::Ensemble(){
        addToLeft(makeNumberOutput(&m_ensemble.input.frequencyOut, "Frequency Out"));
        addToLeft(makeSoundInput(&m_ensemble.input));
        addToLeft(makeNumberInput(&m_ensemble.frequencyIn, "Frequency In"));
        addToLeft(makeNumberInput(&m_ensemble.frequencySpread, "Spread Factor"));
        addtoRight(makeSoundOutput(&m_ensemble));
        setBody(makeSimpleBody("Ensemble", 0x99BB22FF));
    }
    RegisterFactoryObject(Ensemble, "Ensemble");

} // namespace flui
