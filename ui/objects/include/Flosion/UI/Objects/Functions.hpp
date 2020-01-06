#pragma once

#include <Flosion/UI/Core/NumberObject.hpp>
#include <Flosion/Objects/Functions.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <GUI/Helpers/Slider.hpp>


namespace flui {

    class Constant : public NumberObject {
    public:
        Constant();

        static std::unique_ptr<Constant> parseConstant(const std::vector<std::string>&);

        double getValue() const noexcept;
        void setValue(double);

    private:
        // TODO: shift-click or double-click or button to edit value
        // TODO: easier way to drag

        flo::Constant m_constant;
        ui::Text* m_label;
        flo::Connection m_conn;
    };

    class Slider : public NumberObject {
    public:
        Slider();

        static std::unique_ptr<Slider> parseSlider(const std::vector<std::string>&);

        double getValue() const noexcept;
        double getMinimum() const noexcept;
        double getMaximum() const noexcept;

        void setValue(double);
        void setMinimum(double);
        void setMaximum(double);

    private:
        // TODO: shift-click or double-click or button to edit value and limits
        // TODO: easier way to drag

        flo::Constant m_constant;
        ui::Slider<double>* m_slider;
        flo::Connection m_conn;
    };

    class Add : public NumberObject {
    public:
        Add();

    private:
        flo::Add m_add;
    };

    class Subtract : public NumberObject {
    public:
        Subtract();

    private:
        flo::Subtract m_subtract;
    };

    class Multiply : public NumberObject {
    public:
        Multiply();

    private:
        flo::Multiply m_multiply;
    };

    class Divide : public NumberObject {
    public:
        Divide();

    private:
        flo::Divide m_divide;
    };

    class PiConstant : public NumberObject {
    public:
        PiConstant();

    private:
        flo::PiConstant m_piConstant;
    };

    class EulersConstant : public NumberObject {
    public:
        EulersConstant();

    private:
        flo::EulersConstant m_eulersConstant;
    };

    class TauConstant : public NumberObject {
    public:
        TauConstant();

    private:
        flo::TauConstant m_tauConstant;
    };

    class SampleFrequencyConstant : public NumberObject {
    public:
        SampleFrequencyConstant();

    private:
        flo::SampleFrequencyConstant m_sfreq;
    };

    class Abs : public NumberObject {
    public:
        Abs();

    private:
        flo::Abs m_abs;
    };

    class SquareRoot : public NumberObject {
    public:
        SquareRoot();

    private:
        flo::SquareRoot m_squareRoot;
    };

    class CubeRoot : public NumberObject {
    public:
        CubeRoot();

    private:
        flo::CubeRoot m_cubeRoot;
    };

    class Square : public NumberObject {
    public:
        Square();

    private:
        flo::Square m_square;
    };

    class Log : public NumberObject {
    public:
        Log();

    private:
        flo::Log m_log;
    };

    class Log2 : public NumberObject {
    public:
        Log2();

    private:
        flo::Log2 m_log2;
    };

    class Log10 : public NumberObject {
    public:
        Log10();

    private:
        flo::Log10 m_log10;
    };

    class Exp : public NumberObject {
    public:
        Exp();

    private:
        flo::Exp m_exp;
    };

    class Exp2 : public NumberObject {
    public:
        Exp2();

    private:
        flo::Exp2 m_exp2;
    };

    class Exp10 : public NumberObject {
    public:
        Exp10();

    private:
        flo::Exp10 m_exp10;
    };

    class Sin : public NumberObject {
    public:
        Sin();

    private:
        flo::Sin m_sin;
    };

    class Cos : public NumberObject {
    public:
        Cos();

    private:
        flo::Cos m_cos;
    };

    class Tan : public NumberObject {
    public:
        Tan();

    private:
        flo::Tan m_tan;
    };

    class Asin : public NumberObject {
    public:
        Asin();

    private:
        flo::Asin m_asin;
    };

    class Acos : public NumberObject {
    public:
        Acos();

    private:
        flo::Acos m_acos;
    };

    class Atan : public NumberObject {
    public:
        Atan();

    private:
        flo::Atan m_atan;
    };

    class Sinh : public NumberObject {
    public:
        Sinh();

    private:
        flo::Sinh m_sinh;
    };

    class Cosh : public NumberObject {
    public:
        Cosh();

    private:
        flo::Cosh m_cosh;
    };

    class Tanh : public NumberObject {
    public:
        Tanh();

    private:
        flo::Tanh m_tanh;
    };

    class Asinh : public NumberObject {
    public:
        Asinh();

    private:
        flo::Asinh m_asinh;
    };

    class Acosh : public NumberObject {
    public:
        Acosh();

    private:
        flo::Acosh m_acosh;
    };

    class Atanh : public NumberObject {
    public:
        Atanh();

    private:
        flo::Atanh m_atanh;
    };

    class Ceil : public NumberObject {
    public:
        Ceil();

    private:
        flo::Ceil m_ceil;
    };

    class Floor : public NumberObject {
    public:
        Floor();

    private:
        flo::Floor m_floor;
    };

    class Round : public NumberObject {
    public:
        Round();

    private:
        flo::Round m_round;
    };

    class Frac : public NumberObject {
    public:
        Frac();

    private:
        flo::Frac m_frac;
    };

    class PlusOne : public NumberObject {
    public:
        PlusOne();

    private:
        flo::PlusOne m_plusOne;
    };

    class MinusOne : public NumberObject {
    public:
        MinusOne();

    private:
        flo::MinusOne m_minusOne;
    };

    class OneMinus : public NumberObject {
    public:
        OneMinus();

    private:
        flo::OneMinus m_oneMinus;
    };
    
    class Negate : public NumberObject {
    public:
        Negate();

    private:
        flo::Negate m_negate;
    };

    class Reciprocal : public NumberObject {
    public:
        Reciprocal();

    private:
        flo::Reciprocal m_reciprocal;
    };

    class StdToNorm : public NumberObject {
    public:
        StdToNorm();

    private:
        flo::StdToNorm m_stdToNorm;
    };

    class NormToStd : public NumberObject {
    public:
        NormToStd();

    private:
        flo::NormToStd m_normToStd;
    };

    class Sigmoid : public NumberObject {
    public:
        Sigmoid();

    private:
        flo::Sigmoid m_sigmoid;
    };

    class Min : public NumberObject {
    public:
        Min();

    private:
        flo::Min m_min;
    };

    class Max : public NumberObject {
    public:
        Max();

    private:
        flo::Max m_max;
    };

    class Pow : public NumberObject {
    public:
        Pow();

    private:
        flo::Pow m_pow;
    };

    class LogBase : public NumberObject {
    public:
        LogBase();

    private:
        flo::LogBase m_logBase;
    };

    class Hypot : public NumberObject {
    public:
        Hypot();

    private:
        flo::Hypot m_hypot;
    };

    class Atan2 : public NumberObject {
    public:
        Atan2();

    private:
        flo::Atan2 m_atan2;
    };

    class RandomUniform : public NumberObject {
    public:
        RandomUniform();

    private:
        flo::RandomUniform m_randomUniform;
    };

    class RandomNormal : public NumberObject {
    public:
        RandomNormal();

    private:
        flo::RandomNormal m_randomNormal;
    };

    class RoundTo : public NumberObject {
    public:
        RoundTo();

    private:
        flo::RoundTo m_roundTo;
    };

    class FloorTo : public NumberObject {
    public:
        FloorTo();

    private:
        flo::FloorTo m_floorTo;
    };

    class CeilTo : public NumberObject {
    public:
        CeilTo();

    private:
        flo::CeilTo m_ceilTo;
    };

    class Remainder : public NumberObject {
    public:
        Remainder();

    private:
        flo::Remainder m_remainder;
    };

    class Gaussian : public NumberObject {
    public:
        Gaussian();

    private:
        flo::Gaussian m_gaussian;
    };

    class LinearInterpolation : public NumberObject {
    public:
        LinearInterpolation();

    private:
        flo::LinearInterpolation m_linearInterpolation;
    };

} // namespace flui
