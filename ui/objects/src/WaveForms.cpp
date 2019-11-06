#include <Flosion/UI/Objects/WaveForms.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    SineWave::SineWave(){
        addToLeft(makeNumberInput(&m_sineWave.input));
        addToRight(makeNumberOutput(&m_sineWave));

        setBody(makeSimpleBody("Sine Wave", 0x402060FF));
    }
    RegisterFactoryObject(SineWave, "Sine Wave", "Unit Sine");

    SawWave::SawWave(){
        addToLeft(makeNumberInput(&m_sawWave.input));
        addToRight(makeNumberOutput(&m_sawWave));

        setBody(makeSimpleBody("Saw Wave", 0x402060FF));
    }
    RegisterFactoryObject(SawWave, "Saw Wave");

    SquareWave::SquareWave(){
        addToLeft(makeNumberInput(&m_squareWave.input));
        addToRight(makeNumberOutput(&m_squareWave));

        setBody(makeSimpleBody("Square Wave", 0x402060FF));
    }
    RegisterFactoryObject(SquareWave, "Square Wave");

    TriangleWave::TriangleWave(){
        addToLeft(makeNumberInput(&m_triangleWave.input));
        addToRight(makeNumberOutput(&m_triangleWave));

        setBody(makeSimpleBody("Triangle Wave", 0x402060FF));
    }
    RegisterFactoryObject(TriangleWave, "Triangle Wave");

} // namespace flui
