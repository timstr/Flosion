#include <Flosion/UI/Objects/WaveForms.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    SineWave::SineWave(){
        addToLeft(makePeg(&m_sineWave.input));
        addToRight(makePeg(&m_sineWave));

        setBody(makeSimpleBody("Sine Wave", 0x402060FF));
    }
    RegisterFactoryObject(SineWave, "Sine Wave", "Unit Sine");

    SawWave::SawWave(){
        addToLeft(makePeg(&m_sawWave.input));
        addToRight(makePeg(&m_sawWave));

        setBody(makeSimpleBody("Saw Wave", 0x402060FF));
    }
    RegisterFactoryObject(SawWave, "Saw Wave");

    SquareWave::SquareWave(){
        addToLeft(makePeg(&m_squareWave.input));
        addToRight(makePeg(&m_squareWave));

        setBody(makeSimpleBody("Square Wave", 0x402060FF));
    }
    RegisterFactoryObject(SquareWave, "Square Wave");

    TriangleWave::TriangleWave(){
        addToLeft(makePeg(&m_triangleWave.input));
        addToRight(makePeg(&m_triangleWave));

        setBody(makeSimpleBody("Triangle Wave", 0x402060FF));
    }
    RegisterFactoryObject(TriangleWave, "Triangle Wave");

} // namespace flui
