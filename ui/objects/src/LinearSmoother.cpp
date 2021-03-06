#include <Flosion/UI/Objects/LinearSmoother.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    LinearSmoother::LinearSmoother()
        : BorrowingNumberObject(&m_smoother) {
    
        addToInflow(makePeg(&m_smoother.input, "Input"));
        addToInflow(makePeg(&m_smoother.rate, "Rate"));
        addToOutflow(makePeg(&m_smoother, "Output"));

        setBody(makeSimpleBody("Linear Smoother", 0xd9d514ff));
    }

    void LinearSmoother::serialize(Serializer& s) const {
        throw SerializationException{};
        // TODO
    }

    void LinearSmoother::deserialize(Deserializer& d) {
        throw SerializationException{};
        // TODO
    }

    RegisterFactoryObject(LinearSmoother, "LinearSmoother");
    REGISTER_SERIALIZABLE(LinearSmoother, "LinearSmoother");

} // namespace flui
