#include <Flosion/UI/Objects/Lowpass.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Lowpass::Lowpass(){
        addToLeft(makeNumberInput(&m_lowpass.cutoff, "Cutoff"));
        addToLeft(makeSoundInput(&m_lowpass.input, "Input"));
        addToTop(makeNumberOutput(&m_lowpass.currentTime, "Current Time"));
        addToRight(makeSoundOutput(&m_lowpass));
        setBody(makeSimpleBody("Lowpass", 0x96471aFF));
    }
    RegisterFactoryObject(Lowpass, "Lowpass");

} // namespace flui
