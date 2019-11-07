#include <Flosion/UI/Objects/Resampler.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Resampler::Resampler(){
        addToTop(makeNumberOutput(&m_resampler.currentTime, "Current Time"));
        addToLeft(makeNumberInput(&m_resampler.timeSpeed, "Time Speed"));
        addToLeft(makeSoundInput(&m_resampler.input));
        addToRight(makeSoundOutput(&m_resampler));
        setBody(makeSimpleBody("Resampler", 0xd47b00ff));
    }
    RegisterFactoryObject(Resampler, "Resampler");

} // namespace flui
