#include <Flosion/UI/Objects/Functions.hpp>

#include <Flosion/UI/Core/ArgumentParser.hpp>
#include <Flosion/UI/Core/Font.hpp>

namespace flui {

    Add::Add(){
        addToLeft(makeNumberInput(&m_add.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_add.input2, "Input 2"));
        addtoRight(makeNumberOutput(&m_add, "Output"));
        setBody(makeSimpleBody("Add"));
    }
    RegisterFactoryObject(Add, "add", "+");

    Subtract::Subtract(){
        addToLeft(makeNumberInput(&m_subtract.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_subtract.input2, "Input 2"));
        addtoRight(makeNumberOutput(&m_subtract, "Output"));
        setBody(makeSimpleBody("Subtract"));
    }
    RegisterFactoryObject(Subtract, "subtract", "-");

    Multiply::Multiply(){
        addToLeft(makeNumberInput(&m_multiply.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_multiply.input2, "Input 2"));
        addtoRight(makeNumberOutput(&m_multiply, "Output"));
        setBody(makeSimpleBody("Multiply"));
    }
    RegisterFactoryObject(Multiply, "multiply", "*");

    Divide::Divide(){
        addToLeft(makeNumberInput(&m_divide.input1, "Numerator"));
        addToLeft(makeNumberInput(&m_divide.input2, "Denominator"));
        addtoRight(makeNumberOutput(&m_divide, "Output"));
        setBody(makeSimpleBody("Divide"));
    }
    RegisterFactoryObject(Divide, "divide", "/");


    Constant::Constant(){
        addtoRight(makeNumberOutput(&m_constant));
        auto b = std::make_unique<ui::Boxed<ui::FreeContainer>>();
        b->setBackgroundColor(0x202040FF);
        b->setBorderColor(0xFFFFFFFF);
        b->setBorderThickness(2.0f);
        m_label = &b->add<ui::Text>(
            ui::FreeContainer::Center,
            ui::FreeContainer::Center,
            std::to_string(m_constant.getValue()),
            getFont(),
            0xFFFFFFFF);
        setBody(std::move(b));
        m_constant.flo::Reactable<flo::Constant, flo::ConstantReactor>::attachReactor(this);
    }

    std::unique_ptr<Constant> Constant::parseConstant(const std::string& s){
        auto p = ArgumentParser{};
        
        std::optional<double> v;
        p.add(v);

        p.parse(s);

        auto c = std::make_unique<Constant>();
        c->getConstant().setValue(v.value_or(0.0f));

        return c;
    }

    flo::Constant& Constant::getConstant() noexcept {
        return m_constant;
    }

    void Constant::onChangeValue(double v){
        assert(m_label);
        m_label->setText(std::to_string(v));
    }

    RegisterFactoryObjectEx(Constant, "constant", Constant::parseConstant); // TODO: pass arguments to factory

} // namespace flui
