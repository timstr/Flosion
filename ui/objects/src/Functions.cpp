#include <Flosion/UI/Objects/Functions.hpp>

#include <Flosion/UI/Core/ArgumentParser.hpp>
#include <Flosion/UI/Core/Font.hpp>

namespace flui {

    Add::Add(){
        addToLeft(makeNumberInput(&m_add.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_add.input2, "Input 2"));
        addtoRight(makeNumberOutput(&m_add, "Output"));
        setBody(makeSimpleBody("Add", 0x202040FF));
    }
    RegisterFactoryObject(Add, "add", "+");

    Subtract::Subtract(){
        addToLeft(makeNumberInput(&m_subtract.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_subtract.input2, "Input 2"));
        addtoRight(makeNumberOutput(&m_subtract, "Output"));
        setBody(makeSimpleBody("Subtract", 0x202040FF));
    }
    RegisterFactoryObject(Subtract, "subtract", "-");

    Multiply::Multiply(){
        addToLeft(makeNumberInput(&m_multiply.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_multiply.input2, "Input 2"));
        addtoRight(makeNumberOutput(&m_multiply, "Output"));
        setBody(makeSimpleBody("Multiply", 0x202040FF));
    }
    RegisterFactoryObject(Multiply, "multiply", "*");

    Divide::Divide(){
        addToLeft(makeNumberInput(&m_divide.input1, "Numerator"));
        addToLeft(makeNumberInput(&m_divide.input2, "Denominator"));
        addtoRight(makeNumberOutput(&m_divide, "Output"));
        setBody(makeSimpleBody("Divide", 0x202040FF));
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
        c->setValue(v.value_or(0.0f));

        return c;
    }

    double Constant::getValue() const noexcept {
        return m_constant.getValue();
    }

    void Constant::setValue(double v){
        m_constant.setValue(v);
    }

    void Constant::onChangeValue(double v){
        assert(m_label);
        m_label->setText(std::to_string(v));
    }

    RegisterFactoryObjectEx(Constant, "constant", Constant::parseConstant);

    Slider::Slider(){
        addtoRight(makeNumberOutput(&m_constant));

        auto sp = std::make_unique<ui::Slider<double>>(
            0.0,
            0.0,
            1.0,
            getFont(),
            [&](double v){
                m_constant.setValue(v);
            }
        );

        m_slider = sp.get();
        auto b = std::make_unique<ui::Boxed<ui::FreeContainer>>();
        b->setMinSize(sp->size() + ui::vec2{10.0f, 10.0f});
        b->setBackgroundColor(0x202040FF);
        b->setBorderColor(0xFFFFFFFF);
        b->setBorderThickness(2.0f);
        b->adopt(
            ui::FreeContainer::Center,
            ui::FreeContainer::Center,
            std::move(sp)
        );
        setBody(std::move(b));

        m_constant.Reactable<flo::Constant, flo::ConstantReactor>::attachReactor(this);
    }

    std::unique_ptr<Slider> Slider::parseSlider(const std::string& s){
        auto p = ArgumentParser{};
        
        std::optional<double> v1;
        std::optional<double> v2;
        std::optional<double> v3;
        p.add(v1);
        p.add(v2);
        p.add(v3);

        p.parse(s);
        
        auto c = std::make_unique<Slider>();

        if (v3){
            assert(v1);
            assert(v2);
            c->setMinimum(*v1);
            c->setMaximum(*v2);
            c->setValue(*v3);
        } else if (v2){
            assert(v1);
            c->setMinimum(*v1);
            c->setMaximum(*v2);
            c->setValue((*v1 + *v2) * 0.5);
        } else if (v1){
            c->setMinimum(0.0);
            c->setMaximum(*v1);
            c->setValue(*v1 * 0.5);
        }

        return c;
    }

    double Slider::getValue() const noexcept {
        return m_constant.getValue();
    }

    double Slider::getMinimum() const noexcept {
        return m_slider->minimum();
    }

    double Slider::getMaximum() const noexcept {
        return m_slider->maximum();
    }

    void Slider::setValue(double v){
        m_constant.setValue(v);
    }

    void Slider::setMinimum(double v){
        m_slider->setMinimum(v);
    }

    void Slider::setMaximum(double v){
        m_slider->setMaximum(v);
    }

    void Slider::onChangeValue(double v){
        m_slider->setValue(v);
    }

    RegisterFactoryObjectEx(Slider, "slider", Slider::parseSlider);

} // namespace flui
