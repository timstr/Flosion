#include <Flosion/UI/Objects/Functions.hpp>

#include <Flosion/UI/Core/Font.hpp>

namespace flui {

    Add::Add(){
        //addToLeft(makeNumberInput(&m_add.input1, "Input 1"));
        //addToLeft(makeNumberInput(&m_add.input2, "Input 2"));
        //addtoRight(makeNumberOutput(&m_add, "Output"));
        setBody(makeSimpleBody("Add"));
    }
    RegisterFactoryObject(Add, "add", "+");

    Subtract::Subtract(){
        //addToLeft(makeNumberInput(&m_subtract.input1, "Input 1"));
        //addToLeft(makeNumberInput(&m_subtract.input2, "Input 2"));
        //addtoRight(makeNumberOutput(&m_subtract, "Output"));
        setBody(makeSimpleBody("Subtract"));
    }
    RegisterFactoryObject(Subtract, "subtract", "-");

    Multiply::Multiply(){
        //addToLeft(makeNumberInput(&m_multiply.input1, "Input 1"));
        //addToLeft(makeNumberInput(&m_multiply.input2, "Input 2"));
        //addtoRight(makeNumberOutput(&m_multiply, "Output"));
        setBody(makeSimpleBody("Multiply"));
    }
    RegisterFactoryObject(Multiply, "multiply", "*");

    Divide::Divide(){
        //addToLeft(makeNumberInput(&m_divide.input1, "Numerator"));
        //addToLeft(makeNumberInput(&m_divide.input2, "Denominator"));
        //addtoRight(makeNumberOutput(&m_divide, "Output"));
        setBody(makeSimpleBody("Divide"));
    }
    RegisterFactoryObject(Divide, "divide", "/");


} // namespace flui
