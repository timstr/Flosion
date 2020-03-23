#include <Flosion/UI/Objects/Accumulator.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Accumulator::Accumulator()
        : BorrowingNumberObject(&m_accumulator) {
    
        addToInflow(makePeg(&m_accumulator.input, "Input"));
        addToInflow(makePeg(&m_accumulator.reset, "Reset"));
        addToOutflow(makePeg(&m_accumulator, "Output"));

        setBody(makeSimpleBody("Accumulator", 0xd9d514ff));
    }

    void Accumulator::serialize(Serializer& s) const {
        throw SerializationException{};
        // TODO
    }

    void Accumulator::deserialize(Deserializer& d) {
        throw SerializationException{};
        // TODO
    }

    RegisterFactoryObject(Accumulator, "Accumulator");

} // namespace flui
