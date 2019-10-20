#include <Flosion/UI/Objects/Functions.hpp>

#include <Flosion/UI/Core/Font.hpp>

namespace flui {

    namespace detail {
        std::unique_ptr<ui::Element> makeBody(ui::String s){
            auto b = std::make_unique<ui::Boxed<ui::FreeContainer>>();
            b->setBackgroundColor(0x202040FF);
            b->setBorderColor(0xFFFFFFFF);
            b->setBorderThickness(2.0f);
            auto& c = b->add<ui::Text>(s, getFont(), 0xFFFFFFFF);
            // TODO: use container margin instead
            b->setMinWidth(c.width() + 50.0f);
            return b;
        }
    }

    Add::Add(){
        addNumberInput(&m_add.input1, "Input 1");
        addNumberInput(&m_add.input2, "Input 2");
        addNumberOutput(&m_add, "Output");
        setBody(detail::makeBody("Add"));
    }
    RegisterFactoryObject(Add, "add", "+");

    Subtract::Subtract(){
        addNumberInput(&m_subtract.input1, "Input 1");
        addNumberInput(&m_subtract.input2, "Input 2");
        addNumberOutput(&m_subtract, "Output");
        setBody(detail::makeBody("Subtract"));
    }
    RegisterFactoryObject(Subtract, "subtract", "-");

    Multiply::Multiply(){
        addNumberInput(&m_multiply.input1, "Input 1");
        addNumberInput(&m_multiply.input2, "Input 2");
        addNumberOutput(&m_multiply, "Output");
        setBody(detail::makeBody("Multiply"));
    }
    RegisterFactoryObject(Multiply, "multiply", "*");

    Divide::Divide(){
        addNumberInput(&m_divide.input1, "Numerator");
        addNumberInput(&m_divide.input2, "Denominator");
        addNumberOutput(&m_divide, "Output");
        setBody(detail::makeBody("Divide"));
    }
    RegisterFactoryObject(Divide, "divide", "/");


} // namespace flui
