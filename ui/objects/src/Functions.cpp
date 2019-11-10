#include <Flosion/UI/Objects/Functions.hpp>

#include <Flosion/UI/Core/ArgumentParser.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>

namespace flui {

    Constant::Constant(){
        addToRight(makePeg(&m_constant));
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

        m_conn = m_constant.onChangeValue.connect([&](double v){
            assert(m_label);
            m_label->setText(std::to_string(v));
        });
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

    RegisterFactoryObjectEx(Constant, "constant", Constant::parseConstant);



    Slider::Slider(){
        addToRight(makePeg(&m_constant));

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

        m_conn = m_constant.onChangeValue.connect([&](double v){
            m_slider->setValue(v);
        });
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

    RegisterFactoryObjectEx(Slider, "slider", Slider::parseSlider);



    Add::Add(){
        addToLeft(makePeg(&m_add.input1, "Input 1"));
        addToLeft(makePeg(&m_add.input2, "Input 2"));
        addToRight(makePeg(&m_add, "Output"));
        setBody(makeSimpleBody("Add", 0x202040FF));
    }
    RegisterFactoryObject(Add, "add", "+");

    Subtract::Subtract(){
        addToLeft(makePeg(&m_subtract.input1, "Input 1"));
        addToLeft(makePeg(&m_subtract.input2, "Input 2"));
        addToRight(makePeg(&m_subtract, "Output"));
        setBody(makeSimpleBody("Subtract", 0x202040FF));
    }
    RegisterFactoryObject(Subtract, "subtract", "-");

    Multiply::Multiply(){
        addToLeft(makePeg(&m_multiply.input1, "Input 1"));
        addToLeft(makePeg(&m_multiply.input2, "Input 2"));
        addToRight(makePeg(&m_multiply, "Output"));
        setBody(makeSimpleBody("Multiply", 0x202040FF));
    }
    RegisterFactoryObject(Multiply, "multiply", "*");

    Divide::Divide(){
        addToLeft(makePeg(&m_divide.input1, "Numerator"));
        addToLeft(makePeg(&m_divide.input2, "Denominator"));
        addToRight(makePeg(&m_divide, "Output"));
        setBody(makeSimpleBody("Divide", 0x202040FF));
    }
    RegisterFactoryObject(Divide, "divide", "/");




    PiConstant::PiConstant(){
        addToRight(makePeg(&m_piConstant));
        setBody(makeSimpleBody("Pi", 0x404040FF));
    }
    RegisterFactoryObject(PiConstant, "pi");
    
    EulersConstant::EulersConstant(){
        addToRight(makePeg(&m_eulersConstant));
        setBody(makeSimpleBody("e", 0x404040FF));
    }
    RegisterFactoryObject(EulersConstant, "e");
    
    TauConstant::TauConstant(){
        addToRight(makePeg(&m_tauConstant));
        setBody(makeSimpleBody("tau", 0x404040FF));
    }
    RegisterFactoryObject(TauConstant, "tau");

    Abs::Abs(){
        addToLeft(makePeg(&m_abs.input));
        addToRight(makePeg(&m_abs));
        setBody(makeSimpleBody("abs", 0x2a3d4dff));
    }
    RegisterFactoryObject(Abs, "abs");

    SquareRoot::SquareRoot(){
        addToLeft(makePeg(&m_squareRoot.input));
        addToRight(makePeg(&m_squareRoot));
        setBody(makeSimpleBody("sqrt", 0x4a4b5eff));
    }
    RegisterFactoryObject(SquareRoot, "sqrt");

    CubeRoot::CubeRoot(){
        addToLeft(makePeg(&m_cubeRoot.input));
        addToRight(makePeg(&m_cubeRoot));
        setBody(makeSimpleBody("cbrt", 0x4a4b5eff));
    }
    RegisterFactoryObject(CubeRoot, "cbrt");

    Square::Square(){
        addToLeft(makePeg(&m_square.input));
        addToRight(makePeg(&m_square));
        setBody(makeSimpleBody("sqr", 0x520a73ff));
    }
    RegisterFactoryObject(Square, "sqr");

    Log::Log(){
        addToLeft(makePeg(&m_log.input));
        addToRight(makePeg(&m_log));
        setBody(makeSimpleBody("log", 0x335238ff));
    }
    RegisterFactoryObject(Log, "log");

    Log2::Log2(){
        addToLeft(makePeg(&m_log2.input));
        addToRight(makePeg(&m_log2));
        setBody(makeSimpleBody("log2", 0x335238ff));
    }
    RegisterFactoryObject(Log2, "log2");

    Log10::Log10(){
        addToLeft(makePeg(&m_log10.input));
        addToRight(makePeg(&m_log10));
        setBody(makeSimpleBody("log10", 0x335238ff));
    }
    RegisterFactoryObject(Log10, "log10");

    Exp::Exp(){
        addToLeft(makePeg(&m_exp.input));
        addToRight(makePeg(&m_exp));
        setBody(makeSimpleBody("exp", 0x7d1a66ff));
    }
    RegisterFactoryObject(Exp, "exp");

    Exp2::Exp2(){
        addToLeft(makePeg(&m_exp2.input));
        addToRight(makePeg(&m_exp2));
        setBody(makeSimpleBody("exp2", 0x622863ff));
    }
    RegisterFactoryObject(Exp2, "exp2");

    Exp10::Exp10(){
        addToLeft(makePeg(&m_exp10.input));
        addToRight(makePeg(&m_exp10));
        setBody(makeSimpleBody("exp10", 0x701453ff));
    }
    RegisterFactoryObject(Exp10, "exp10");

    Sin::Sin(){
        addToLeft(makePeg(&m_sin.input));
        addToRight(makePeg(&m_sin));
        setBody(makeSimpleBody("sin", 0x735b0bff));
    }
    RegisterFactoryObject(Sin, "sin");

    Cos::Cos(){
        addToLeft(makePeg(&m_cos.input));
        addToRight(makePeg(&m_cos));
        setBody(makeSimpleBody("cos", 0x735b0bff));
    }
    RegisterFactoryObject(Cos, "cos");

    Tan::Tan(){
        addToLeft(makePeg(&m_tan.input));
        addToRight(makePeg(&m_tan));
        setBody(makeSimpleBody("tan", 0x735b0bff));
    }
    RegisterFactoryObject(Tan, "tan");

    Asin::Asin(){
        addToLeft(makePeg(&m_asin.input));
        addToRight(makePeg(&m_asin));
        setBody(makeSimpleBody("asin", 0xc7b350ff));
    }
    RegisterFactoryObject(Asin, "asin");

    Acos::Acos(){
        addToLeft(makePeg(&m_acos.input));
        addToRight(makePeg(&m_acos));
        setBody(makeSimpleBody("acos", 0xc7b350ff));
    }
    RegisterFactoryObject(Acos, "acos");

    Atan::Atan(){
        addToLeft(makePeg(&m_atan.input));
        addToRight(makePeg(&m_atan));
        setBody(makeSimpleBody("atan", 0xc7b350ff));
    }
    RegisterFactoryObject(Atan, "atan");

    Sinh::Sinh(){
        addToLeft(makePeg(&m_sinh.input));
        addToRight(makePeg(&m_sinh));
        setBody(makeSimpleBody("sinh", 0x497d47ff));
    }
    RegisterFactoryObject(Sinh, "sinh");

    Cosh::Cosh(){
        addToLeft(makePeg(&m_cosh.input));
        addToRight(makePeg(&m_cosh));
        setBody(makeSimpleBody("cosh", 0x497d47ff));
    }
    RegisterFactoryObject(Cosh, "cosh");

    Tanh::Tanh(){
        addToLeft(makePeg(&m_tanh.input));
        addToRight(makePeg(&m_tanh));
        setBody(makeSimpleBody("tanh", 0x497d47ff));
    }
    RegisterFactoryObject(Tanh, "tanh");

    Asinh::Asinh(){
        addToLeft(makePeg(&m_asinh.input));
        addToRight(makePeg(&m_asinh));
        setBody(makeSimpleBody("asinh", 0x647d63ff));
    }
    RegisterFactoryObject(Asinh, "asinh");

    Acosh::Acosh(){
        addToLeft(makePeg(&m_acosh.input));
        addToRight(makePeg(&m_acosh));
        setBody(makeSimpleBody("acosh", 0x647d63ff));
    }
    RegisterFactoryObject(Acosh, "acosh");

    Atanh::Atanh(){
        addToLeft(makePeg(&m_atanh.input));
        addToRight(makePeg(&m_atanh));
        setBody(makeSimpleBody("abs", 0x647d63ff));
    }
    RegisterFactoryObject(Atanh, "atanh");

    Ceil::Ceil(){
        addToLeft(makePeg(&m_ceil.input));
        addToRight(makePeg(&m_ceil));
        setBody(makeSimpleBody("ceil", 0x36367dff));
    }
    RegisterFactoryObject(Ceil, "ceil");

    Floor::Floor(){
        addToLeft(makePeg(&m_floor.input));
        addToRight(makePeg(&m_floor));
        setBody(makeSimpleBody("floor", 0x36367dff));
    }
    RegisterFactoryObject(Floor, "floor");

    Round::Round(){
        addToLeft(makePeg(&m_round.input));
        addToRight(makePeg(&m_round));
        setBody(makeSimpleBody("round", 0x36367dff));
    }
    RegisterFactoryObject(Round, "round");

    Frac::Frac(){
        addToLeft(makePeg(&m_frac.input));
        addToRight(makePeg(&m_frac));
        setBody(makeSimpleBody("frac", 0x1e5924ff));
    }
    RegisterFactoryObject(Frac, "frac");

    PlusOne::PlusOne(){
        addToLeft(makePeg(&m_plusOne.input));
        addToRight(makePeg(&m_plusOne));
        setBody(makeSimpleBody("+1", 0x786538ff));
    }
    RegisterFactoryObject(PlusOne, "plusone", "+1");

    MinusOne::MinusOne(){
        addToLeft(makePeg(&m_minusOne.input));
        addToRight(makePeg(&m_minusOne));
        setBody(makeSimpleBody("-1", 0x786538ff));
    }
    RegisterFactoryObject(MinusOne, "minusone", "-1");

    OneMinus::OneMinus(){
        addToLeft(makePeg(&m_oneMinus.input));
        addToRight(makePeg(&m_oneMinus));
        setBody(makeSimpleBody("1-", 0x786538ff));
    }
    RegisterFactoryObject(OneMinus, "oneminus", "1-");

    Negate::Negate(){
        addToLeft(makePeg(&m_negate.input));
        addToRight(makePeg(&m_negate));
        setBody(makeSimpleBody("negate", 0x732a0aff));
    }
    RegisterFactoryObject(Negate, "negate");

    Reciprocal::Reciprocal(){
        addToLeft(makePeg(&m_reciprocal.input));
        addToRight(makePeg(&m_reciprocal));
        setBody(makeSimpleBody("reciprocal", 0x732a0aff));
    }
    RegisterFactoryObject(Reciprocal, "reciprocal");

    StdToNorm::StdToNorm(){
        addToLeft(makePeg(&m_stdToNorm.input));
        addToRight(makePeg(&m_stdToNorm));
        setBody(makeSimpleBody("Std to Norm", 0x546e66ff));
    }
    RegisterFactoryObject(StdToNorm, "StdToNorm");

    NormToStd::NormToStd(){
        addToLeft(makePeg(&m_normToStd.input));
        addToRight(makePeg(&m_normToStd));
        setBody(makeSimpleBody("Norm to Std", 0x546e66ff));
    }
    RegisterFactoryObject(NormToStd, "NormToStd");

    Sigmoid::Sigmoid(){
        addToLeft(makePeg(&m_sigmoid.input));
        addToRight(makePeg(&m_sigmoid));
        setBody(makeSimpleBody("sigmoid", 0xaba552ff));
    }
    RegisterFactoryObject(Sigmoid, "sigmoid");

    Min::Min(){
        addToLeft(makePeg(&m_min.input1));
        addToLeft(makePeg(&m_min.input2));
        addToRight(makePeg(&m_min));
        setBody(makeSimpleBody("min", 0x535773ff));
    }
    RegisterFactoryObject(Min, "min");

    Max::Max(){
        addToLeft(makePeg(&m_max.input1));
        addToLeft(makePeg(&m_max.input2));
        addToRight(makePeg(&m_max));
        setBody(makeSimpleBody("max", 0x535773ff));
    }
    RegisterFactoryObject(Max, "max");

    Pow::Pow(){
        addToLeft(makePeg(&m_pow.input1, "Base"));
        addToLeft(makePeg(&m_pow.input2, "Exponent"));
        addToRight(makePeg(&m_pow));
        setBody(makeSimpleBody("pow", 0x691500ff));
    }
    RegisterFactoryObject(Pow, "pow");

    LogBase::LogBase(){
        addToLeft(makePeg(&m_logBase.input1, "Input"));
        addToLeft(makePeg(&m_logBase.input2, "Base"));
        addToRight(makePeg(&m_logBase));
        setBody(makeSimpleBody("Log Base", 0x59194bff));
    }
    RegisterFactoryObject(LogBase, "logbase");

    Hypot::Hypot(){
        addToLeft(makePeg(&m_hypot.input1));
        addToLeft(makePeg(&m_hypot.input2));
        addToRight(makePeg(&m_hypot));
        setBody(makeSimpleBody("hypot", 0x335950ff));
    }
    RegisterFactoryObject(Hypot, "hypot");

    Atan2::Atan2(){
        addToLeft(makePeg(&m_atan2.input1, "X"));
        addToLeft(makePeg(&m_atan2.input2, "Y"));
        addToRight(makePeg(&m_atan2));
        setBody(makeSimpleBody("atan2", 0x335950ff));
    }
    RegisterFactoryObject(Atan2, "atan2");

    RandomUniform::RandomUniform(){
        addToLeft(makePeg(&m_randomUniform.input1, "Minimum"));
        addToLeft(makePeg(&m_randomUniform.input2, "Maximum"));
        addToRight(makePeg(&m_randomUniform));
        setBody(makeSimpleBody("Random Uniform", 0xcfc800ff));
    }
    RegisterFactoryObject(RandomUniform, "RandomUniform");

    RandomNormal::RandomNormal(){
        addToLeft(makePeg(&m_randomNormal.input1, "Mean"));
        addToLeft(makePeg(&m_randomNormal.input2, "Standard Deviation"));
        addToRight(makePeg(&m_randomNormal));
        setBody(makeSimpleBody("Random Normal", 0xcfc800ff));
    }
    RegisterFactoryObject(RandomNormal, "RandomNormal");

    RoundTo::RoundTo(){
        addToLeft(makePeg(&m_roundTo.input1, "Input"));
        addToLeft(makePeg(&m_roundTo.input2, "Base"));
        addToRight(makePeg(&m_roundTo));
        setBody(makeSimpleBody("Round To", 0x452c5cff));
    }
    RegisterFactoryObject(RoundTo, "RoundTo");

    FloorTo::FloorTo(){
        addToLeft(makePeg(&m_floorTo.input1, "Input"));
        addToLeft(makePeg(&m_floorTo.input2, "Base"));
        addToRight(makePeg(&m_floorTo));
        setBody(makeSimpleBody("Floor To", 0x452c5cff));
    }
    RegisterFactoryObject(FloorTo, "FloorTo");

    CeilTo::CeilTo(){
        addToLeft(makePeg(&m_ceilTo.input1, "Input"));
        addToLeft(makePeg(&m_ceilTo.input2, "Base"));
        addToRight(makePeg(&m_ceilTo));
        setBody(makeSimpleBody("Ceil To", 0x452c5cff));
    }
    RegisterFactoryObject(CeilTo, "CeilTo");

    Remainder::Remainder(){
        addToLeft(makePeg(&m_remainder.input1, "Numerator"));
        addToLeft(makePeg(&m_remainder.input2, "Denominator"));
        addToRight(makePeg(&m_remainder));
        setBody(makeSimpleBody("Remainder", 0x6b4c6aff));
    }
    RegisterFactoryObject(Remainder, "Remainder");

    Gaussian::Gaussian(){
        addToLeft(makePeg(&m_gaussian.input, "Input"));
        addToLeft(makePeg(&m_gaussian.amplitude, "Amplitude"));
        addToLeft(makePeg(&m_gaussian.center, "Center"));
        addToLeft(makePeg(&m_gaussian.width, "Width"));
        addToRight(makePeg(&m_gaussian));
        setBody(makeSimpleBody("Gaussian", 0x87807cff));
    }
    RegisterFactoryObject(Gaussian, "Gaussian");

    LinearInterpolation::LinearInterpolation(){
        addToLeft(makePeg(&m_linearInterpolation.start, "Start"));
        addToLeft(makePeg(&m_linearInterpolation.end, "End"));
        addToLeft(makePeg(&m_linearInterpolation.fraction, "Fraction"));
        addToRight(makePeg(&m_linearInterpolation));
        setBody(makeSimpleBody("lerp", 0x2f4857ff));
    }
    RegisterFactoryObject(LinearInterpolation, "LinearInterpolation", "lerp");


} // namespace flui
