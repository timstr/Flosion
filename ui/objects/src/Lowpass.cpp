#include <Flosion/UI/Objects/Lowpass.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Lowpass::Lowpass(){
        addToLeft(makePeg(&m_lowpass.cutoff, "Cutoff"));
        addToLeft(makePeg(&m_lowpass.input, "Input"));
        addToTop(makePeg(&m_lowpass.currentTime, "Current Time"));
        addToRight(makePeg(&m_lowpass));
        setBody(makeSimpleBody("Lowpass", 0x96471aFF));
    }
    RegisterFactoryObject(Lowpass, "Lowpass");

} // namespace flui
