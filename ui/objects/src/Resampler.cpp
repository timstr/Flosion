#include <Flosion/UI/Objects/Resampler.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Resampler::Resampler(){
        addToTop(makePeg(&m_resampler.currentTime, "Current Time"));
        addToLeft(makePeg(&m_resampler.timeSpeed, "Time Speed"));
        addToLeft(makePeg(&m_resampler.input));
        addToRight(makePeg(&m_resampler));
        setBody(makeSimpleBody("Resampler", 0xd47b00ff));
    }
    RegisterFactoryObject(Resampler, "Resampler");

} // namespace flui
