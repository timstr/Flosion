#include <Flosion/UI/Objects/Lowpass.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Lowpass::Lowpass()
        : SoundObject(&m_lowpass) {
        addToInflow(makePeg(&m_lowpass.cutoff, "Cutoff"));
        addToInflow(makePeg(&m_lowpass.input, "Input"));
        addToInflow(makePeg(&m_lowpass.currentTime, "Current Time"));
        addToOutflow(makePeg(&m_lowpass));
        setBody(makeSimpleBody("Lowpass", 0x96471aFF));
    }
    RegisterFactoryObject(Lowpass, "Lowpass");

} // namespace flui
