#include <Flosion/UI/Objects/Ensemble.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Ensemble::Ensemble(){
        addToLeft(makePeg(&m_ensemble.input.frequencyOut, "Frequency Out"));
        addToLeft(makePeg(&m_ensemble.input));
        addToLeft(makePeg(&m_ensemble.frequencyIn, "Frequency In"));
        addToLeft(makePeg(&m_ensemble.frequencySpread, "Spread Factor"));
        addToRight(makePeg(&m_ensemble));
        setBody(makeSimpleBody("Ensemble", 0x99BB22FF));
    }
    RegisterFactoryObject(Ensemble, "Ensemble");

} // namespace flui
