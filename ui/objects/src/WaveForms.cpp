#include <Flosion/UI/Objects/WaveForms.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    SineWave::SineWave(){
        addToInflow(makePeg(&m_sineWave.input));
        addToOutflow(makePeg(&m_sineWave));

        setBody(makeSimpleBody("Sine Wave", 0x402060FF));
    }
    RegisterFactoryObject(SineWave, "Sine Wave", "Unit Sine");

    SawWave::SawWave(){
        addToInflow(makePeg(&m_sawWave.input));
        addToOutflow(makePeg(&m_sawWave));

        setBody(makeSimpleBody("Saw Wave", 0x402060FF));
    }
    RegisterFactoryObject(SawWave, "Saw Wave");

    SquareWave::SquareWave(){
        addToInflow(makePeg(&m_squareWave.input));
        addToOutflow(makePeg(&m_squareWave));

        setBody(makeSimpleBody("Square Wave", 0x402060FF));
    }
    RegisterFactoryObject(SquareWave, "Square Wave");

    TriangleWave::TriangleWave(){
        addToInflow(makePeg(&m_triangleWave.input));
        addToOutflow(makePeg(&m_triangleWave));

        setBody(makeSimpleBody("Triangle Wave", 0x402060FF));
    }
    RegisterFactoryObject(TriangleWave, "Triangle Wave");

} // namespace flui
