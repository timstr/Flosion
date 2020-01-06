#include <Flosion/UI/Objects/FlipFlop.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    FlipFlop::FlipFlop()
        : BorrowingNumberObject(&m_flipFlop) {
    
        addToInflow(makePeg(&m_flipFlop.input, "Input"));
        addToInflow(makePeg(&m_flipFlop.clock, "Clock"));
        addToOutflow(makePeg(&m_flipFlop, "Output"));

        setBody(makeSimpleBody("FlipFlop", 0xd9d514ff));
    }

    RegisterFactoryObject(FlipFlop, "FlipFlop");

} // namespace flui
