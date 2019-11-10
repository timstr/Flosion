#pragma once

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Functions.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <GUI/Helpers/Slider.hpp>


namespace flui {

    class Constant : public Object {
    public:
        Constant();

        static std::unique_ptr<Constant> parseConstant(const std::string&);

        double getValue() const noexcept;
        void setValue(double);

    private:
        // TODO: shift-click or double-click or button to edit value
        // TODO: easier way to drag

        flo::Constant m_constant;
        ui::Text* m_label;
        flo::Connection m_conn;
    };

    class Slider : public Object {
    public:
        Slider();

        static std::unique_ptr<Slider> parseSlider(const std::string&);

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

    class Add : public Object {
    public:
        Add();

    private:
        flo::Add m_add;
    };

    class Subtract : public Object {
    public:
        Subtract();

    private:
        flo::Subtract m_subtract;
    };

    class Multiply : public Object {
    public:
        Multiply();

    private:
        flo::Multiply m_multiply;
    };

    class Divide : public Object {
    public:
        Divide();

    private:
        flo::Divide m_divide;
    };

    class PiConstant : public Object {
    public:
        PiConstant();

    private:
        flo::PiConstant m_piConstant;
    };

    class EulersConstant : public Object {
    public:
        EulersConstant();

    private:
        flo::EulersConstant m_eulersConstant;
    };

    class TauConstant : public Object {
    public:
        TauConstant();

    private:
        flo::TauConstant m_tauConstant;
    };

    class Abs : public Object {
    public:
        Abs();

    private:
        flo::Abs m_abs;
    };

    class SquareRoot : public Object {
    public:
        SquareRoot();

    private:
        flo::SquareRoot m_squareRoot;
    };

    class CubeRoot : public Object {
    public:
        CubeRoot();

    private:
        flo::CubeRoot m_cubeRoot;
    };

    class Square : public Object {
    public:
        Square();

    private:
        flo::Square m_square;
    };

    class Log : public Object {
    public:
        Log();

    private:
        flo::Log m_log;
    };

    class Log2 : public Object {
    public:
        Log2();

    private:
        flo::Log2 m_log2;
    };

    class Log10 : public Object {
    public:
        Log10();

    private:
        flo::Log10 m_log10;
    };

    class Exp : public Object {
    public:
        Exp();

    private:
        flo::Exp m_exp;
    };

    class Exp2 : public Object {
    public:
        Exp2();

    private:
        flo::Exp2 m_exp2;
    };

    class Exp10 : public Object {
    public:
        Exp10();

    private:
        flo::Exp10 m_exp10;
    };

    class Sin : public Object {
    public:
        Sin();

    private:
        flo::Sin m_sin;
    };

    class Cos : public Object {
    public:
        Cos();

    private:
        flo::Cos m_cos;
    };

    class Tan : public Object {
    public:
        Tan();

    private:
        flo::Tan m_tan;
    };

    class Asin : public Object {
    public:
        Asin();

    private:
        flo::Asin m_asin;
    };

    class Acos : public Object {
    public:
        Acos();

    private:
        flo::Acos m_acos;
    };

    class Atan : public Object {
    public:
        Atan();

    private:
        flo::Atan m_atan;
    };

    class Sinh : public Object {
    public:
        Sinh();

    private:
        flo::Sinh m_sinh;
    };

    class Cosh : public Object {
    public:
        Cosh();

    private:
        flo::Cosh m_cosh;
    };

    class Tanh : public Object {
    public:
        Tanh();

    private:
        flo::Tanh m_tanh;
    };

    class Asinh : public Object {
    public:
        Asinh();

    private:
        flo::Asinh m_asinh;
    };

    class Acosh : public Object {
    public:
        Acosh();

    private:
        flo::Acosh m_acosh;
    };

    class Atanh : public Object {
    public:
        Atanh();

    private:
        flo::Atanh m_atanh;
    };

    class Ceil : public Object {
    public:
        Ceil();

    private:
        flo::Ceil m_ceil;
    };

    class Floor : public Object {
    public:
        Floor();

    private:
        flo::Floor m_floor;
    };

    class Round : public Object {
    public:
        Round();

    private:
        flo::Round m_round;
    };

    class Frac : public Object {
    public:
        Frac();

    private:
        flo::Frac m_frac;
    };

    class PlusOne : public Object {
    public:
        PlusOne();

    private:
        flo::PlusOne m_plusOne;
    };

    class MinusOne : public Object {
    public:
        MinusOne();

    private:
        flo::MinusOne m_minusOne;
    };

    class OneMinus : public Object {
    public:
        OneMinus();

    private:
        flo::OneMinus m_oneMinus;
    };
    
    class Negate : public Object {
    public:
        Negate();

    private:
        flo::Negate m_negate;
    };

    class Reciprocal : public Object {
    public:
        Reciprocal();

    private:
        flo::Reciprocal m_reciprocal;
    };

    class StdToNorm : public Object {
    public:
        StdToNorm();

    private:
        flo::StdToNorm m_stdToNorm;
    };

    class NormToStd : public Object {
    public:
        NormToStd();

    private:
        flo::NormToStd m_normToStd;
    };

    class Sigmoid : public Object {
    public:
        Sigmoid();

    private:
        flo::Sigmoid m_sigmoid;
    };

    class Min : public Object {
    public:
        Min();

    private:
        flo::Min m_min;
    };

    class Max : public Object {
    public:
        Max();

    private:
        flo::Max m_max;
    };

    class Pow : public Object {
    public:
        Pow();

    private:
        flo::Pow m_pow;
    };

    class LogBase : public Object {
    public:
        LogBase();

    private:
        flo::LogBase m_logBase;
    };

    class Hypot : public Object {
    public:
        Hypot();

    private:
        flo::Hypot m_hypot;
    };

    class Atan2 : public Object {
    public:
        Atan2();

    private:
        flo::Atan2 m_atan2;
    };

    class RandomUniform : public Object {
    public:
        RandomUniform();

    private:
        flo::RandomUniform m_randomUniform;
    };

    class RandomNormal : public Object {
    public:
        RandomNormal();

    private:
        flo::RandomNormal m_randomNormal;
    };

    class RoundTo : public Object {
    public:
        RoundTo();

    private:
        flo::RoundTo m_roundTo;
    };

    class FloorTo : public Object {
    public:
        FloorTo();

    private:
        flo::FloorTo m_floorTo;
    };

    class CeilTo : public Object {
    public:
        CeilTo();

    private:
        flo::CeilTo m_ceilTo;
    };

    class Remainder : public Object {
    public:
        Remainder();

    private:
        flo::Remainder m_remainder;
    };

    class Gaussian : public Object {
    public:
        Gaussian();

    private:
        flo::Gaussian m_gaussian;
    };

    class LinearInterpolation : public Object {
    public:
        LinearInterpolation();

    private:
        flo::LinearInterpolation m_linearInterpolation;
    };

} // namespace flui
