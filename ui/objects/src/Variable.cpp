#include <Flosion/UI/Objects/Variable.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Variable::Variable()
        : BorrowingNumberObject(&m_variable) {
    
        addToInflow(makePeg(&m_variable.input, "Input"));
        addToOutflow(makePeg(&m_variable, "Output"));

        setBody(makeSimpleBody("Variable", 0xd9d514ff));
    }

    void Variable::serialize(Serializer& s) const {
        throw SerializationException{};
        // TODO
    }

    void Variable::deserialize(Deserializer& d) {
        throw SerializationException{};
        // TODO
    }

    RegisterFactoryObject(Variable, "Variable");
    REGISTER_SERIALIZABLE(Variable, "Variable");

} // namespace flui
