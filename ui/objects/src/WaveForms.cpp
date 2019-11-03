#include <Flosion/UI/Objects/WaveForms.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    SineWave::SineWave(){
        addToLeft(makeNumberInput(&m_sineWave.input));
        addtoRight(makeNumberOutput(&m_sineWave));

        setBody(makeSimpleBody("Sine Wave"));
    }
    RegisterFactoryObject(SineWave, "Sine Wave", "Unit Sine");

    SawWave::SawWave(){
        addToLeft(makeNumberInput(&m_sawWave.input));
        addtoRight(makeNumberOutput(&m_sawWave));

        setBody(makeSimpleBody("Saw Wave"));
    }
    RegisterFactoryObject(SawWave, "Saw Wave");

    SquareWave::SquareWave(){
        addToLeft(makeNumberInput(&m_squareWave.input));
        addtoRight(makeNumberOutput(&m_squareWave));

        setBody(makeSimpleBody("Square Wave"));
    }
    RegisterFactoryObject(SquareWave, "Square Wave");

    TriangleWave::TriangleWave(){
        addToLeft(makeNumberInput(&m_triangleWave.input));
        addtoRight(makeNumberOutput(&m_triangleWave));

        setBody(makeSimpleBody("Triangle Wave"));
    }
    RegisterFactoryObject(TriangleWave, "Triangle Wave");

} // namespace flui
