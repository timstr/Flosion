#include <Flosion/UI/Objects/Accumulator.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Accumulator::Accumulator()
        : BorrowingNumberObject(&m_accumulator) {
    
        addToInflow(makePeg(&m_accumulator.input, "Input"));
        addToOutflow(makePeg(&m_accumulator, "Output"));

        setBody(makeSimpleBody("Accumulator", 0xd9d514ff));
    }

    RegisterFactoryObject(Accumulator, "Accumulator");

} // namespace flui
