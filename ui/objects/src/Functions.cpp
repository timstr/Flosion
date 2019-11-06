#include <Flosion/UI/Objects/Functions.hpp>

#include <Flosion/UI/Core/ArgumentParser.hpp>
#include <Flosion/UI/Core/Font.hpp>

namespace flui {

    Constant::Constant(){
        addToRight(makeNumberOutput(&m_constant));
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
        addToRight(makeNumberOutput(&m_constant));

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



    Add::Add(){
        addToLeft(makeNumberInput(&m_add.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_add.input2, "Input 2"));
        addToRight(makeNumberOutput(&m_add, "Output"));
        setBody(makeSimpleBody("Add", 0x202040FF));
    }
    RegisterFactoryObject(Add, "add", "+");

    Subtract::Subtract(){
        addToLeft(makeNumberInput(&m_subtract.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_subtract.input2, "Input 2"));
        addToRight(makeNumberOutput(&m_subtract, "Output"));
        setBody(makeSimpleBody("Subtract", 0x202040FF));
    }
    RegisterFactoryObject(Subtract, "subtract", "-");

    Multiply::Multiply(){
        addToLeft(makeNumberInput(&m_multiply.input1, "Input 1"));
        addToLeft(makeNumberInput(&m_multiply.input2, "Input 2"));
        addToRight(makeNumberOutput(&m_multiply, "Output"));
        setBody(makeSimpleBody("Multiply", 0x202040FF));
    }
    RegisterFactoryObject(Multiply, "multiply", "*");

    Divide::Divide(){
        addToLeft(makeNumberInput(&m_divide.input1, "Numerator"));
        addToLeft(makeNumberInput(&m_divide.input2, "Denominator"));
        addToRight(makeNumberOutput(&m_divide, "Output"));
        setBody(makeSimpleBody("Divide", 0x202040FF));
    }
    RegisterFactoryObject(Divide, "divide", "/");




    PiConstant::PiConstant(){
        addToRight(makeNumberOutput(&m_piConstant));
        setBody(makeSimpleBody("Pi", 0x404040FF));
    }
    RegisterFactoryObject(PiConstant, "pi");
    
    EulersConstant::EulersConstant(){
        addToRight(makeNumberOutput(&m_eulersConstant));
        setBody(makeSimpleBody("e", 0x404040FF));
    }
    RegisterFactoryObject(EulersConstant, "e");
    
    TauConstant::TauConstant(){
        addToRight(makeNumberOutput(&m_tauConstant));
        setBody(makeSimpleBody("tau", 0x404040FF));
    }
    RegisterFactoryObject(TauConstant, "tau");

    Abs::Abs(){
        addToLeft(makeNumberInput(&m_abs.input));
        addToRight(makeNumberOutput(&m_abs));
        setBody(makeSimpleBody("abs", 0x2a3d4dff));
    }
    RegisterFactoryObject(Abs, "abs");

    SquareRoot::SquareRoot(){
        addToLeft(makeNumberInput(&m_squareRoot.input));
        addToRight(makeNumberOutput(&m_squareRoot));
        setBody(makeSimpleBody("sqrt", 0x4a4b5eff));
    }
    RegisterFactoryObject(SquareRoot, "sqrt");

    CubeRoot::CubeRoot(){
        addToLeft(makeNumberInput(&m_cubeRoot.input));
        addToRight(makeNumberOutput(&m_cubeRoot));
        setBody(makeSimpleBody("cbrt", 0x4a4b5eff));
    }
    RegisterFactoryObject(CubeRoot, "cbrt");

    Square::Square(){
        addToLeft(makeNumberInput(&m_square.input));
        addToRight(makeNumberOutput(&m_square));
        setBody(makeSimpleBody("sqr", 0x520a73ff));
    }
    RegisterFactoryObject(Square, "sqr");

    Log::Log(){
        addToLeft(makeNumberInput(&m_log.input));
        addToRight(makeNumberOutput(&m_log));
        setBody(makeSimpleBody("log", 0x335238ff));
    }
    RegisterFactoryObject(Log, "log");

    Log2::Log2(){
        addToLeft(makeNumberInput(&m_log2.input));
        addToRight(makeNumberOutput(&m_log2));
        setBody(makeSimpleBody("log2", 0x335238ff));
    }
    RegisterFactoryObject(Log2, "log2");

    Log10::Log10(){
        addToLeft(makeNumberInput(&m_log10.input));
        addToRight(makeNumberOutput(&m_log10));
        setBody(makeSimpleBody("log10", 0x335238ff));
    }
    RegisterFactoryObject(Log10, "log10");

    Exp::Exp(){
        addToLeft(makeNumberInput(&m_exp.input));
        addToRight(makeNumberOutput(&m_exp));
        setBody(makeSimpleBody("exp", 0x7d1a66ff));
    }
    RegisterFactoryObject(Exp, "exp");

    Exp2::Exp2(){
        addToLeft(makeNumberInput(&m_exp2.input));
        addToRight(makeNumberOutput(&m_exp2));
        setBody(makeSimpleBody("exp2", 0x622863ff));
    }
    RegisterFactoryObject(Exp2, "exp2");

    Exp10::Exp10(){
        addToLeft(makeNumberInput(&m_exp10.input));
        addToRight(makeNumberOutput(&m_exp10));
        setBody(makeSimpleBody("exp10", 0x701453ff));
    }
    RegisterFactoryObject(Exp10, "exp10");

    Sin::Sin(){
        addToLeft(makeNumberInput(&m_sin.input));
        addToRight(makeNumberOutput(&m_sin));
        setBody(makeSimpleBody("sin", 0x735b0bff));
    }
    RegisterFactoryObject(Sin, "sin");

    Cos::Cos(){
        addToLeft(makeNumberInput(&m_cos.input));
        addToRight(makeNumberOutput(&m_cos));
        setBody(makeSimpleBody("cos", 0x735b0bff));
    }
    RegisterFactoryObject(Cos, "cos");

    Tan::Tan(){
        addToLeft(makeNumberInput(&m_tan.input));
        addToRight(makeNumberOutput(&m_tan));
        setBody(makeSimpleBody("tan", 0x735b0bff));
    }
    RegisterFactoryObject(Tan, "tan");

    Asin::Asin(){
        addToLeft(makeNumberInput(&m_asin.input));
        addToRight(makeNumberOutput(&m_asin));
        setBody(makeSimpleBody("asin", 0xc7b350ff));
    }
    RegisterFactoryObject(Asin, "asin");

    Acos::Acos(){
        addToLeft(makeNumberInput(&m_acos.input));
        addToRight(makeNumberOutput(&m_acos));
        setBody(makeSimpleBody("acos", 0xc7b350ff));
    }
    RegisterFactoryObject(Acos, "acos");

    Atan::Atan(){
        addToLeft(makeNumberInput(&m_atan.input));
        addToRight(makeNumberOutput(&m_atan));
        setBody(makeSimpleBody("atan", 0xc7b350ff));
    }
    RegisterFactoryObject(Atan, "atan");

    Sinh::Sinh(){
        addToLeft(makeNumberInput(&m_sinh.input));
        addToRight(makeNumberOutput(&m_sinh));
        setBody(makeSimpleBody("sinh", 0x497d47ff));
    }
    RegisterFactoryObject(Sinh, "sinh");

    Cosh::Cosh(){
        addToLeft(makeNumberInput(&m_cosh.input));
        addToRight(makeNumberOutput(&m_cosh));
        setBody(makeSimpleBody("cosh", 0x497d47ff));
    }
    RegisterFactoryObject(Cosh, "cosh");

    Tanh::Tanh(){
        addToLeft(makeNumberInput(&m_tanh.input));
        addToRight(makeNumberOutput(&m_tanh));
        setBody(makeSimpleBody("tanh", 0x497d47ff));
    }
    RegisterFactoryObject(Tanh, "tanh");

    Asinh::Asinh(){
        addToLeft(makeNumberInput(&m_asinh.input));
        addToRight(makeNumberOutput(&m_asinh));
        setBody(makeSimpleBody("asinh", 0x647d63ff));
    }
    RegisterFactoryObject(Asinh, "asinh");

    Acosh::Acosh(){
        addToLeft(makeNumberInput(&m_acosh.input));
        addToRight(makeNumberOutput(&m_acosh));
        setBody(makeSimpleBody("acosh", 0x647d63ff));
    }
    RegisterFactoryObject(Acosh, "acosh");

    Atanh::Atanh(){
        addToLeft(makeNumberInput(&m_atanh.input));
        addToRight(makeNumberOutput(&m_atanh));
        setBody(makeSimpleBody("abs", 0x647d63ff));
    }
    RegisterFactoryObject(Atanh, "abs");

    Ceil::Ceil(){
        addToLeft(makeNumberInput(&m_ceil.input));
        addToRight(makeNumberOutput(&m_ceil));
        setBody(makeSimpleBody("ceil", 0x36367dff));
    }
    RegisterFactoryObject(Ceil, "ceil");

    Floor::Floor(){
        addToLeft(makeNumberInput(&m_floor.input));
        addToRight(makeNumberOutput(&m_floor));
        setBody(makeSimpleBody("floor", 0x36367dff));
    }
    RegisterFactoryObject(Floor, "floor");

    Round::Round(){
        addToLeft(makeNumberInput(&m_round.input));
        addToRight(makeNumberOutput(&m_round));
        setBody(makeSimpleBody("round", 0x36367dff));
    }
    RegisterFactoryObject(Round, "round");

    Frac::Frac(){
        addToLeft(makeNumberInput(&m_frac.input));
        addToRight(makeNumberOutput(&m_frac));
        setBody(makeSimpleBody("frac", 0x1e5924ff));
    }
    RegisterFactoryObject(Frac, "frac");

    PlusOne::PlusOne(){
        addToLeft(makeNumberInput(&m_plusOne.input));
        addToRight(makeNumberOutput(&m_plusOne));
        setBody(makeSimpleBody("+1", 0x786538ff));
    }
    RegisterFactoryObject(PlusOne, "plusone", "-1");

    MinusOne::MinusOne(){
        addToLeft(makeNumberInput(&m_minusOne.input));
        addToRight(makeNumberOutput(&m_minusOne));
        setBody(makeSimpleBody("-1", 0x786538ff));
    }
    RegisterFactoryObject(MinusOne, "minusone", "-1");

    OneMinus::OneMinus(){
        addToLeft(makeNumberInput(&m_oneMinus.input));
        addToRight(makeNumberOutput(&m_oneMinus));
        setBody(makeSimpleBody("-1", 0x786538ff));
    }
    RegisterFactoryObject(OneMinus, "oneminus", "1-");

    Negate::Negate(){
        addToLeft(makeNumberInput(&m_negate.input));
        addToRight(makeNumberOutput(&m_negate));
        setBody(makeSimpleBody("negate", 0x732a0aff));
    }
    RegisterFactoryObject(Negate, "negate");

    Reciprocal::Reciprocal(){
        addToLeft(makeNumberInput(&m_reciprocal.input));
        addToRight(makeNumberOutput(&m_reciprocal));
        setBody(makeSimpleBody("reciprocal", 0x732a0aff));
    }
    RegisterFactoryObject(Reciprocal, "reciprocal");

    StdToNorm::StdToNorm(){
        addToLeft(makeNumberInput(&m_stdToNorm.input));
        addToRight(makeNumberOutput(&m_stdToNorm));
        setBody(makeSimpleBody("Std to Norm", 0x546e66ff));
    }
    RegisterFactoryObject(StdToNorm, "StdToNorm");

    NormToStd::NormToStd(){
        addToLeft(makeNumberInput(&m_normToStd.input));
        addToRight(makeNumberOutput(&m_normToStd));
        setBody(makeSimpleBody("Norm to Std", 0x546e66ff));
    }
    RegisterFactoryObject(NormToStd, "NormToStd");

    Sigmoid::Sigmoid(){
        addToLeft(makeNumberInput(&m_sigmoid.input));
        addToRight(makeNumberOutput(&m_sigmoid));
        setBody(makeSimpleBody("sigmoid", 0xaba552ff));
    }
    RegisterFactoryObject(Sigmoid, "sigmoid");

    Min::Min(){
        addToLeft(makeNumberInput(&m_min.input1));
        addToLeft(makeNumberInput(&m_min.input2));
        addToRight(makeNumberOutput(&m_min));
        setBody(makeSimpleBody("min", 0x535773ff));
    }
    RegisterFactoryObject(Min, "min");

    Max::Max(){
        addToLeft(makeNumberInput(&m_max.input1));
        addToLeft(makeNumberInput(&m_max.input2));
        addToRight(makeNumberOutput(&m_max));
        setBody(makeSimpleBody("max", 0x535773ff));
    }
    RegisterFactoryObject(Max, "max");

    Pow::Pow(){
        addToLeft(makeNumberInput(&m_pow.input1, "Base"));
        addToLeft(makeNumberInput(&m_pow.input2, "Exponent"));
        addToRight(makeNumberOutput(&m_pow));
        setBody(makeSimpleBody("pow", 0x691500ff));
    }
    RegisterFactoryObject(Pow, "pow");

    LogBase::LogBase(){
        addToLeft(makeNumberInput(&m_logBase.input1, "Input"));
        addToLeft(makeNumberInput(&m_logBase.input2, "Base"));
        addToRight(makeNumberOutput(&m_logBase));
        setBody(makeSimpleBody("Log Base", 0x59194bff));
    }
    RegisterFactoryObject(LogBase, "logbase");

    Hypot::Hypot(){
        addToLeft(makeNumberInput(&m_hypot.input1));
        addToLeft(makeNumberInput(&m_hypot.input2));
        addToRight(makeNumberOutput(&m_hypot));
        setBody(makeSimpleBody("hypot", 0x335950ff));
    }
    RegisterFactoryObject(Hypot, "hypot");

    Atan2::Atan2(){
        addToLeft(makeNumberInput(&m_atan2.input1, "X"));
        addToLeft(makeNumberInput(&m_atan2.input2, "Y"));
        addToRight(makeNumberOutput(&m_atan2));
        setBody(makeSimpleBody("atan2", 0x335950ff));
    }
    RegisterFactoryObject(Atan2, "atan2");

    RandomUniform::RandomUniform(){
        addToLeft(makeNumberInput(&m_randomUniform.input1, "Minimum"));
        addToLeft(makeNumberInput(&m_randomUniform.input2, "Maximum"));
        addToRight(makeNumberOutput(&m_randomUniform));
        setBody(makeSimpleBody("Random Uniform", 0xcfc800));
    }
    RegisterFactoryObject(RandomUniform, "RandomUniform");

    RandomNormal::RandomNormal(){
        addToLeft(makeNumberInput(&m_randomNormal.input1, "Mean"));
        addToLeft(makeNumberInput(&m_randomNormal.input2, "Standard Deviation"));
        addToRight(makeNumberOutput(&m_randomNormal));
        setBody(makeSimpleBody("Random Normal", 0xcfc800));
    }
    RegisterFactoryObject(RandomNormal, "RandomNormal");

    RoundTo::RoundTo(){
        addToLeft(makeNumberInput(&m_roundTo.input1, "Input"));
        addToLeft(makeNumberInput(&m_roundTo.input2, "Base"));
        addToRight(makeNumberOutput(&m_roundTo));
        setBody(makeSimpleBody("Round To", 0x452c5cff));
    }
    RegisterFactoryObject(RoundTo, "RoundTo");

    FloorTo::FloorTo(){
        addToLeft(makeNumberInput(&m_floorTo.input1, "Input"));
        addToLeft(makeNumberInput(&m_floorTo.input2, "Base"));
        addToRight(makeNumberOutput(&m_floorTo));
        setBody(makeSimpleBody("Floor To", 0x452c5cff));
    }
    RegisterFactoryObject(FloorTo, "FloorTo");

    CeilTo::CeilTo(){
        addToLeft(makeNumberInput(&m_ceilTo.input1, "Input"));
        addToLeft(makeNumberInput(&m_ceilTo.input2, "Base"));
        addToRight(makeNumberOutput(&m_ceilTo));
        setBody(makeSimpleBody("Ceil To", 0x452c5cff));
    }
    RegisterFactoryObject(CeilTo, "CeilTo");

    Remainder::Remainder(){
        addToLeft(makeNumberInput(&m_remainder.input1, "Numerator"));
        addToLeft(makeNumberInput(&m_remainder.input2, "Denominator"));
        addToRight(makeNumberOutput(&m_remainder));
        setBody(makeSimpleBody("Remainder", 0x6b4c6aff));
    }
    RegisterFactoryObject(Remainder, "Remainder");

    Gaussian::Gaussian(){
        addToLeft(makeNumberInput(&m_gaussian.input, "Input"));
        addToLeft(makeNumberInput(&m_gaussian.amplitude, "Amplitude"));
        addToLeft(makeNumberInput(&m_gaussian.center, "Center"));
        addToLeft(makeNumberInput(&m_gaussian.width, "Width"));
        addToRight(makeNumberOutput(&m_gaussian));
        setBody(makeSimpleBody("Gaussian", 0x87807cff));
    }
    RegisterFactoryObject(Gaussian, "Gaussian");

    LinearInterpolation::LinearInterpolation(){
        addToLeft(makeNumberInput(&m_linearInterpolation.start, "Start"));
        addToLeft(makeNumberInput(&m_linearInterpolation.end, "End"));
        addToLeft(makeNumberInput(&m_linearInterpolation.fraction, "Fraction"));
        addToRight(makeNumberOutput(&m_linearInterpolation));
        setBody(makeSimpleBody("lerp", 0x2f4857ff));
    }
    RegisterFactoryObject(LinearInterpolation, "LinearInterpolation", "lerp");


} // namespace flui
