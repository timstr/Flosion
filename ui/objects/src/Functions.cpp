#include <Flosion/UI/Objects/Functions.hpp>

namespace flui {

    Add::Add(){
        addNumberInput(&m_add.inputA, "Input A");
        addNumberInput(&m_add.inputB, "Input B");
        addNumberOutput(&m_add, "Output");
    }
    
    RegisterFactoryObject(Add, "add", "+", "sum");

} // namespace flui
