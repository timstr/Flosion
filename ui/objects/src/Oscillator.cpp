#include <Flosion/UI/Objects/Oscillator.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Oscillator::Oscillator()
        : BorrowingNumberObject(&m_oscillator) {
    
        addToInflow(makePeg(&m_oscillator.input, "Input"));
        addToOutflow(makePeg(&m_oscillator, "Output"));

        setBody(makeSimpleBody("Oscillator", 0xd9d514ff));
    }

    RegisterFactoryObject(Oscillator, "Oscillator");

} // namespace flui
