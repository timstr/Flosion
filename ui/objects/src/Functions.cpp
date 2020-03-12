#include <Flosion/UI/Objects/Functions.hpp>

#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/ArgumentParser.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>

namespace flui {

    Constant::Constant(){
        addToOutflow(makePeg(&m_constant));
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

    std::unique_ptr<Constant> Constant::parseConstant(const std::vector<std::string>& args){
        auto p = ArgumentParser{};
        
        std::optional<double> v;
        p.add(v);

        p.parse(args);

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

    void Constant::serialize(Serializer& s) const {
        serializePegs(s);
        s.f64(m_constant.getValue()).str(m_label->text().toAnsiString());
    }

    void Constant::deserialize(Deserializer& d){
        deserializePegs(d);
        auto v = d.f64();
        m_constant.setValue(v);
        auto s = d.str();
        m_label->setText(s);
    }

    RegisterFactoryObjectEx(Constant, "constant", Constant::parseConstant);
    REGISTER_SERIALIZABLE(Constant, "Constant");


    Slider::Slider(const ui::String& name)
        : m_list(nullptr)
        , m_label(nullptr) {

        addToOutflow(makePeg(&m_constant));

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

        auto vl = std::make_unique<ui::Boxed<ui::VerticalList>>();
        m_list = vl.get();

        vl->setPadding(5.0f);
        vl->setBackgroundColor(0x202040FF);
        vl->setBorderColor(0xFFFFFFFF);
        vl->setBorderThickness(2.0f);

        if (name.getSize() > 0) {
            setName(name);
        }

        auto& b = vl->push_back<ui::Boxed<ui::HorizontalList>>();
        b.setBackgroundColor(0x0);
        b.setPadding(0.0f);
        auto& spacer = b.push_back<ui::Element>();
        spacer.setSize({20.0f, 20.0f}, true);
        b.push_back(std::move(sp));

        setBody(std::move(vl));

        m_conn = m_constant.onChangeValue.connect([&](double v){
            m_slider->setValue(v);
        });
    }

    std::unique_ptr<Slider> Slider::parseSlider(const std::vector<std::string>& args){
        auto p = ArgumentParser{};
        
        std::optional<double> v1;
        std::optional<double> v2;
        std::optional<double> v3;
        std::optional<std::string> name; // TODO: allow string to contain rest of arguments, not just first word
        p.add(v1);
        p.add(v2);
        p.add(v3);
        p.add(name);

        p.parse(args);
        
        auto c = std::make_unique<Slider>(name ? *name : "");

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

    ui::String Slider::name() const {
        return m_label ? m_label->text() : "";
    }

    void Slider::setName(const ui::String& s){
        if (s.getSize() == 0) {
            removeLabel();
            return;
        }
        if (!m_label) {
            addLabel();
        }
        assert(m_label);
        m_label->setText(s);
    }

    void Slider::addLabel(){
        assert(m_list);
        if (!m_label) {
            m_label = &m_list->push_front<ui::Text>(
                "",
                getFont(),
                0xFFFFFFFF,
                15,
                ui::TextStyle::Italic
            );
        }
    }

    void Slider::removeLabel(){
        if (m_label) {
            m_label->close();
            m_label = nullptr;
        }
    }

    void Slider::serialize(Serializer& s) const {
        serializePegs(s);
        s.f64(m_slider->minimum()).f64(m_slider->maximum()).f64(m_constant.getValue());
        s.str(name().toAnsiString());
    }

    void Slider::deserialize(Deserializer& d) {
        deserializePegs(d);
        auto min = d.f64();
        auto max = d.f64();
        auto v = d.f64();
        auto s = d.str();
        m_slider->setMinimum(min);
        m_slider->setMaximum(max);
        m_constant.setValue(v);
        setName(s);
    }

    RegisterFactoryObjectEx(Slider, "slider", Slider::parseSlider);
    REGISTER_SERIALIZABLE(Slider, "Slider");


    Add::Add(){
        addToInflow(makePeg(&m_add.input1, "Input 1"));
        addToInflow(makePeg(&m_add.input2, "Input 2"));
        addToOutflow(makePeg(&m_add, "Output"));
        setBody(makeSimpleBody("Add", 0x202040FF));
    }
    RegisterFactoryObject(Add, "add", "+");
    REGISTER_SERIALIZABLE(Add, "Add");

    Subtract::Subtract(){
        addToInflow(makePeg(&m_subtract.input1, "Input 1"));
        addToInflow(makePeg(&m_subtract.input2, "Input 2"));
        addToOutflow(makePeg(&m_subtract, "Output"));
        setBody(makeSimpleBody("Subtract", 0x202040FF));
    }
    RegisterFactoryObject(Subtract, "subtract", "-");
    REGISTER_SERIALIZABLE(Subtract, "Subtract");

    Multiply::Multiply(){
        addToInflow(makePeg(&m_multiply.input1, "Input 1"));
        addToInflow(makePeg(&m_multiply.input2, "Input 2"));
        addToOutflow(makePeg(&m_multiply, "Output"));
        setBody(makeSimpleBody("Multiply", 0x202040FF));
    }
    RegisterFactoryObject(Multiply, "multiply", "*");
    REGISTER_SERIALIZABLE(Multiply, "Multiply");

    Divide::Divide(){
        addToInflow(makePeg(&m_divide.input1, "Numerator"));
        addToInflow(makePeg(&m_divide.input2, "Denominator"));
        addToOutflow(makePeg(&m_divide, "Output"));
        setBody(makeSimpleBody("Divide", 0x202040FF));
    }
    RegisterFactoryObject(Divide, "divide", "/");
    REGISTER_SERIALIZABLE(Divide, "Divide");




    PiConstant::PiConstant(){
        addToOutflow(makePeg(&m_piConstant));
        setBody(makeSimpleBody("Pi", 0x404040FF));
    }
    RegisterFactoryObject(PiConstant, "pi");
    REGISTER_SERIALIZABLE(PiConstant, "PiConstant");
    
    EulersConstant::EulersConstant(){
        addToOutflow(makePeg(&m_eulersConstant));
        setBody(makeSimpleBody("e", 0x404040FF));
    }
    RegisterFactoryObject(EulersConstant, "e");
    REGISTER_SERIALIZABLE(EulersConstant, "EulersConstant");
    
    TauConstant::TauConstant(){
        addToOutflow(makePeg(&m_tauConstant));
        setBody(makeSimpleBody("tau", 0x404040FF));
    }
    RegisterFactoryObject(TauConstant, "tau");
    REGISTER_SERIALIZABLE(TauConstant, "TauConstant");

    SampleFrequencyConstant::SampleFrequencyConstant(){
        addToOutflow(makePeg(&m_sfreq));
        setBody(makeSimpleBody("Sample Frequency", 0x2a3d4dff));
    }
    RegisterFactoryObject(SampleFrequencyConstant, "SampleFrequency");
    REGISTER_SERIALIZABLE(SampleFrequencyConstant, "SampleFrequencyConstant");

    Abs::Abs(){
        addToInflow(makePeg(&m_abs.input));
        addToOutflow(makePeg(&m_abs));
        setBody(makeSimpleBody("abs", 0x2a3d4dff));
    }
    RegisterFactoryObject(Abs, "abs");
    REGISTER_SERIALIZABLE(Abs, "Abs");

    SquareRoot::SquareRoot(){
        addToInflow(makePeg(&m_squareRoot.input));
        addToOutflow(makePeg(&m_squareRoot));
        setBody(makeSimpleBody("sqrt", 0x4a4b5eff));
    }
    RegisterFactoryObject(SquareRoot, "sqrt");
    REGISTER_SERIALIZABLE(SquareRoot, "SquareRoot");

    CubeRoot::CubeRoot(){
        addToInflow(makePeg(&m_cubeRoot.input));
        addToOutflow(makePeg(&m_cubeRoot));
        setBody(makeSimpleBody("cbrt", 0x4a4b5eff));
    }
    RegisterFactoryObject(CubeRoot, "cbrt");
    REGISTER_SERIALIZABLE(CubeRoot, "CubeRoot");

    Square::Square(){
        addToInflow(makePeg(&m_square.input));
        addToOutflow(makePeg(&m_square));
        setBody(makeSimpleBody("sqr", 0x520a73ff));
    }
    RegisterFactoryObject(Square, "sqr");
    REGISTER_SERIALIZABLE(Square, "Square");

    Log::Log(){
        addToInflow(makePeg(&m_log.input));
        addToOutflow(makePeg(&m_log));
        setBody(makeSimpleBody("log", 0x335238ff));
    }
    RegisterFactoryObject(Log, "log");
    REGISTER_SERIALIZABLE(Log, "Log");

    Log2::Log2(){
        addToInflow(makePeg(&m_log2.input));
        addToOutflow(makePeg(&m_log2));
        setBody(makeSimpleBody("log2", 0x335238ff));
    }
    RegisterFactoryObject(Log2, "log2");
    REGISTER_SERIALIZABLE(Log2, "Log2");

    Log10::Log10(){
        addToInflow(makePeg(&m_log10.input));
        addToOutflow(makePeg(&m_log10));
        setBody(makeSimpleBody("log10", 0x335238ff));
    }
    RegisterFactoryObject(Log10, "log10");
    REGISTER_SERIALIZABLE(Log10, "Log10");

    Exp::Exp(){
        addToInflow(makePeg(&m_exp.input));
        addToOutflow(makePeg(&m_exp));
        setBody(makeSimpleBody("exp", 0x7d1a66ff));
    }
    RegisterFactoryObject(Exp, "exp");
    REGISTER_SERIALIZABLE(Exp, "Exp");

    Exp2::Exp2(){
        addToInflow(makePeg(&m_exp2.input));
        addToOutflow(makePeg(&m_exp2));
        setBody(makeSimpleBody("exp2", 0x622863ff));
    }
    RegisterFactoryObject(Exp2, "exp2");
    REGISTER_SERIALIZABLE(Exp2, "Exp2");

    Exp10::Exp10(){
        addToInflow(makePeg(&m_exp10.input));
        addToOutflow(makePeg(&m_exp10));
        setBody(makeSimpleBody("exp10", 0x701453ff));
    }
    RegisterFactoryObject(Exp10, "exp10");
    REGISTER_SERIALIZABLE(Exp10, "Exp10");

    Sin::Sin(){
        addToInflow(makePeg(&m_sin.input));
        addToOutflow(makePeg(&m_sin));
        setBody(makeSimpleBody("sin", 0x735b0bff));
    }
    RegisterFactoryObject(Sin, "sin");
    REGISTER_SERIALIZABLE(Sin, "Sin");

    Cos::Cos(){
        addToInflow(makePeg(&m_cos.input));
        addToOutflow(makePeg(&m_cos));
        setBody(makeSimpleBody("cos", 0x735b0bff));
    }
    RegisterFactoryObject(Cos, "cos");
    REGISTER_SERIALIZABLE(Cos, "Cos");

    Tan::Tan(){
        addToInflow(makePeg(&m_tan.input));
        addToOutflow(makePeg(&m_tan));
        setBody(makeSimpleBody("tan", 0x735b0bff));
    }
    RegisterFactoryObject(Tan, "tan");
    REGISTER_SERIALIZABLE(Tan, "Tan");

    Asin::Asin(){
        addToInflow(makePeg(&m_asin.input));
        addToOutflow(makePeg(&m_asin));
        setBody(makeSimpleBody("asin", 0xc7b350ff));
    }
    RegisterFactoryObject(Asin, "asin");
    REGISTER_SERIALIZABLE(Asin, "Asin");

    Acos::Acos(){
        addToInflow(makePeg(&m_acos.input));
        addToOutflow(makePeg(&m_acos));
        setBody(makeSimpleBody("acos", 0xc7b350ff));
    }
    RegisterFactoryObject(Acos, "acos");
    REGISTER_SERIALIZABLE(Acos, "Acos");

    Atan::Atan(){
        addToInflow(makePeg(&m_atan.input));
        addToOutflow(makePeg(&m_atan));
        setBody(makeSimpleBody("atan", 0xc7b350ff));
    }
    RegisterFactoryObject(Atan, "atan");
    REGISTER_SERIALIZABLE(Atan, "Atan");

    Sinh::Sinh(){
        addToInflow(makePeg(&m_sinh.input));
        addToOutflow(makePeg(&m_sinh));
        setBody(makeSimpleBody("sinh", 0x497d47ff));
    }
    RegisterFactoryObject(Sinh, "sinh");
    REGISTER_SERIALIZABLE(Sinh, "Sinh");

    Cosh::Cosh(){
        addToInflow(makePeg(&m_cosh.input));
        addToOutflow(makePeg(&m_cosh));
        setBody(makeSimpleBody("cosh", 0x497d47ff));
    }
    RegisterFactoryObject(Cosh, "cosh");
    REGISTER_SERIALIZABLE(Cosh, "Cosh");

    Tanh::Tanh(){
        addToInflow(makePeg(&m_tanh.input));
        addToOutflow(makePeg(&m_tanh));
        setBody(makeSimpleBody("tanh", 0x497d47ff));
    }
    RegisterFactoryObject(Tanh, "tanh");
    REGISTER_SERIALIZABLE(Tanh, "Tanh");

    Asinh::Asinh(){
        addToInflow(makePeg(&m_asinh.input));
        addToOutflow(makePeg(&m_asinh));
        setBody(makeSimpleBody("asinh", 0x647d63ff));
    }
    RegisterFactoryObject(Asinh, "asinh");
    REGISTER_SERIALIZABLE(Asinh, "Asinh");

    Acosh::Acosh(){
        addToInflow(makePeg(&m_acosh.input));
        addToOutflow(makePeg(&m_acosh));
        setBody(makeSimpleBody("acosh", 0x647d63ff));
    }
    RegisterFactoryObject(Acosh, "acosh");
    REGISTER_SERIALIZABLE(Acosh, "Acosh");

    Atanh::Atanh(){
        addToInflow(makePeg(&m_atanh.input));
        addToOutflow(makePeg(&m_atanh));
        setBody(makeSimpleBody("abs", 0x647d63ff));
    }
    RegisterFactoryObject(Atanh, "atanh");
    REGISTER_SERIALIZABLE(Atanh, "Atanh");

    Ceil::Ceil(){
        addToInflow(makePeg(&m_ceil.input));
        addToOutflow(makePeg(&m_ceil));
        setBody(makeSimpleBody("ceil", 0x36367dff));
    }
    RegisterFactoryObject(Ceil, "ceil");
    REGISTER_SERIALIZABLE(Ceil, "Ceil");

    Floor::Floor(){
        addToInflow(makePeg(&m_floor.input));
        addToOutflow(makePeg(&m_floor));
        setBody(makeSimpleBody("floor", 0x36367dff));
    }
    RegisterFactoryObject(Floor, "floor");
    REGISTER_SERIALIZABLE(Floor, "Floor");

    Round::Round(){
        addToInflow(makePeg(&m_round.input));
        addToOutflow(makePeg(&m_round));
        setBody(makeSimpleBody("round", 0x36367dff));
    }
    RegisterFactoryObject(Round, "round");
    REGISTER_SERIALIZABLE(Round, "Round");

    Frac::Frac(){
        addToInflow(makePeg(&m_frac.input));
        addToOutflow(makePeg(&m_frac));
        setBody(makeSimpleBody("frac", 0x1e5924ff));
    }
    RegisterFactoryObject(Frac, "frac");
    REGISTER_SERIALIZABLE(Frac, "Frac");

    PlusOne::PlusOne(){
        addToInflow(makePeg(&m_plusOne.input));
        addToOutflow(makePeg(&m_plusOne));
        setBody(makeSimpleBody("+1", 0x786538ff));
    }
    RegisterFactoryObject(PlusOne, "plusone", "+1", "1+");
    REGISTER_SERIALIZABLE(PlusOne, "PlusOne");

    MinusOne::MinusOne(){
        addToInflow(makePeg(&m_minusOne.input));
        addToOutflow(makePeg(&m_minusOne));
        setBody(makeSimpleBody("-1", 0x786538ff));
    }
    RegisterFactoryObject(MinusOne, "minusone", "-1");
    REGISTER_SERIALIZABLE(MinusOne, "MinusOne");

    OneMinus::OneMinus(){
        addToInflow(makePeg(&m_oneMinus.input));
        addToOutflow(makePeg(&m_oneMinus));
        setBody(makeSimpleBody("1-", 0x786538ff));
    }
    RegisterFactoryObject(OneMinus, "oneminus", "1-");
    REGISTER_SERIALIZABLE(OneMinus, "OneMinus");

    Negate::Negate(){
        addToInflow(makePeg(&m_negate.input));
        addToOutflow(makePeg(&m_negate));
        setBody(makeSimpleBody("negate", 0x732a0aff));
    }
    RegisterFactoryObject(Negate, "negate");
    REGISTER_SERIALIZABLE(Negate, "Negate");

    Reciprocal::Reciprocal(){
        addToInflow(makePeg(&m_reciprocal.input));
        addToOutflow(makePeg(&m_reciprocal));
        setBody(makeSimpleBody("reciprocal", 0x732a0aff));
    }
    RegisterFactoryObject(Reciprocal, "reciprocal");
    REGISTER_SERIALIZABLE(Reciprocal, "Reciprocal");

    StdToNorm::StdToNorm(){
        addToInflow(makePeg(&m_stdToNorm.input));
        addToOutflow(makePeg(&m_stdToNorm));
        setBody(makeSimpleBody("Std to Norm", 0x546e66ff));
    }
    RegisterFactoryObject(StdToNorm, "StdToNorm");
    REGISTER_SERIALIZABLE(StdToNorm, "StdToNorm");

    NormToStd::NormToStd(){
        addToInflow(makePeg(&m_normToStd.input));
        addToOutflow(makePeg(&m_normToStd));
        setBody(makeSimpleBody("Norm to Std", 0x546e66ff));
    }
    RegisterFactoryObject(NormToStd, "NormToStd");
    REGISTER_SERIALIZABLE(NormToStd, "NormToStd");

    Sigmoid::Sigmoid(){
        addToInflow(makePeg(&m_sigmoid.input));
        addToOutflow(makePeg(&m_sigmoid));
        setBody(makeSimpleBody("sigmoid", 0xaba552ff));
    }
    RegisterFactoryObject(Sigmoid, "sigmoid");
    REGISTER_SERIALIZABLE(Sigmoid, "Sigmoid");

    Min::Min(){
        addToInflow(makePeg(&m_min.input1));
        addToInflow(makePeg(&m_min.input2));
        addToOutflow(makePeg(&m_min));
        setBody(makeSimpleBody("min", 0x535773ff));
    }
    RegisterFactoryObject(Min, "min");
    REGISTER_SERIALIZABLE(Min, "Min");

    Max::Max(){
        addToInflow(makePeg(&m_max.input1));
        addToInflow(makePeg(&m_max.input2));
        addToOutflow(makePeg(&m_max));
        setBody(makeSimpleBody("max", 0x535773ff));
    }
    RegisterFactoryObject(Max, "max");
    REGISTER_SERIALIZABLE(Max, "Max");

    Pow::Pow(){
        addToInflow(makePeg(&m_pow.input1, "Base"));
        addToInflow(makePeg(&m_pow.input2, "Exponent"));
        addToOutflow(makePeg(&m_pow));
        setBody(makeSimpleBody("pow", 0x691500ff));
    }
    RegisterFactoryObject(Pow, "pow");
    REGISTER_SERIALIZABLE(Pow, "Pow");

    LogBase::LogBase(){
        addToInflow(makePeg(&m_logBase.input1, "Input"));
        addToInflow(makePeg(&m_logBase.input2, "Base"));
        addToOutflow(makePeg(&m_logBase));
        setBody(makeSimpleBody("Log Base", 0x59194bff));
    }
    RegisterFactoryObject(LogBase, "logbase");
    REGISTER_SERIALIZABLE(LogBase, "LogBase");

    Hypot::Hypot(){
        addToInflow(makePeg(&m_hypot.input1));
        addToInflow(makePeg(&m_hypot.input2));
        addToOutflow(makePeg(&m_hypot));
        setBody(makeSimpleBody("hypot", 0x335950ff));
    }
    RegisterFactoryObject(Hypot, "hypot");
    REGISTER_SERIALIZABLE(Hypot, "Hypot");

    Atan2::Atan2(){
        addToInflow(makePeg(&m_atan2.input1, "X"));
        addToInflow(makePeg(&m_atan2.input2, "Y"));
        addToOutflow(makePeg(&m_atan2));
        setBody(makeSimpleBody("atan2", 0x335950ff));
    }
    RegisterFactoryObject(Atan2, "atan2");
    REGISTER_SERIALIZABLE(Atan2, "Atan2");

    RandomUniform::RandomUniform(){
        addToInflow(makePeg(&m_randomUniform.input1, "Minimum"));
        addToInflow(makePeg(&m_randomUniform.input2, "Maximum"));
        addToOutflow(makePeg(&m_randomUniform));
        setBody(makeSimpleBody("Random Uniform", 0xcfc800ff));
    }
    RegisterFactoryObject(RandomUniform, "RandomUniform");
    REGISTER_SERIALIZABLE(RandomUniform, "RandomUniform");

    RandomNormal::RandomNormal(){
        addToInflow(makePeg(&m_randomNormal.input1, "Mean"));
        addToInflow(makePeg(&m_randomNormal.input2, "Standard Deviation"));
        addToOutflow(makePeg(&m_randomNormal));
        setBody(makeSimpleBody("Random Normal", 0xcfc800ff));
    }
    RegisterFactoryObject(RandomNormal, "RandomNormal");
    REGISTER_SERIALIZABLE(RandomNormal, "RandomNormal");

    RoundTo::RoundTo(){
        addToInflow(makePeg(&m_roundTo.input1, "Input"));
        addToInflow(makePeg(&m_roundTo.input2, "Base"));
        addToOutflow(makePeg(&m_roundTo));
        setBody(makeSimpleBody("Round To", 0x452c5cff));
    }
    RegisterFactoryObject(RoundTo, "RoundTo");
    REGISTER_SERIALIZABLE(RoundTo, "RoundTo");

    FloorTo::FloorTo(){
        addToInflow(makePeg(&m_floorTo.input1, "Input"));
        addToInflow(makePeg(&m_floorTo.input2, "Base"));
        addToOutflow(makePeg(&m_floorTo));
        setBody(makeSimpleBody("Floor To", 0x452c5cff));
    }
    RegisterFactoryObject(FloorTo, "FloorTo");
    REGISTER_SERIALIZABLE(FloorTo, "FloorTo");

    CeilTo::CeilTo(){
        addToInflow(makePeg(&m_ceilTo.input1, "Input"));
        addToInflow(makePeg(&m_ceilTo.input2, "Base"));
        addToOutflow(makePeg(&m_ceilTo));
        setBody(makeSimpleBody("Ceil To", 0x452c5cff));
    }
    RegisterFactoryObject(CeilTo, "CeilTo");
    REGISTER_SERIALIZABLE(CeilTo, "CeilTo");

    Remainder::Remainder(){
        addToInflow(makePeg(&m_remainder.input1, "Numerator"));
        addToInflow(makePeg(&m_remainder.input2, "Denominator"));
        addToOutflow(makePeg(&m_remainder));
        setBody(makeSimpleBody("Remainder", 0x6b4c6aff));
    }
    RegisterFactoryObject(Remainder, "Remainder");
    REGISTER_SERIALIZABLE(Remainder, "Remainder");

    Gaussian::Gaussian(){
        addToInflow(makePeg(&m_gaussian.input, "Input"));
        addToInflow(makePeg(&m_gaussian.amplitude, "Amplitude"));
        addToInflow(makePeg(&m_gaussian.center, "Center"));
        addToInflow(makePeg(&m_gaussian.width, "Width"));
        addToOutflow(makePeg(&m_gaussian));
        setBody(makeSimpleBody("Gaussian", 0x87807cff));
    }
    RegisterFactoryObject(Gaussian, "Gaussian");
    REGISTER_SERIALIZABLE(Gaussian, "Gaussian");

    LinearInterpolation::LinearInterpolation(){
        addToInflow(makePeg(&m_linearInterpolation.start, "Start"));
        addToInflow(makePeg(&m_linearInterpolation.end, "End"));
        addToInflow(makePeg(&m_linearInterpolation.fraction, "Fraction"));
        addToOutflow(makePeg(&m_linearInterpolation));
        setBody(makeSimpleBody("lerp", 0x2f4857ff));
    }
    RegisterFactoryObject(LinearInterpolation, "LinearInterpolation", "lerp");
    REGISTER_SERIALIZABLE(LinearInterpolation, "LinearInterpolation");

    void TrivialNumberObject::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void TrivialNumberObject::deserialize(Deserializer& d) {
        deserializePegs(d);
    }

} // namespace flui
