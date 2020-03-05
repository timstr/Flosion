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

    // TODO
    // RegisterFactoryObject(Variable, "Variable");

} // namespace flui
