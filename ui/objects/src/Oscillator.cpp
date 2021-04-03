#include <Flosion/UI/Objects/Oscillator.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Oscillator::Oscillator()
        : BorrowingNumberObject(&m_oscillator) {
    
        addToInflow(makePeg(&m_oscillator.input, "Input"));
        addToInflow(makePeg(&m_oscillator.reset, "Reset"));
        addToOutflow(makePeg(&m_oscillator, "Output"));

        setBody(makeSimpleBody("Oscillator", 0xd9d514ff));
    }

    void Oscillator::serialize(Serializer& s) const {
        throw SerializationException{};
        // TODO
    }

    void Oscillator::deserialize(Deserializer& d) {
        throw SerializationException{};
        // TODO
    }

    RegisterFactoryObject(Oscillator, "Oscillator");
    REGISTER_SERIALIZABLE(Oscillator, "Oscillator");

} // namespace flui
