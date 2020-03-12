#pragma once

#include <Flosion/UI/Core/NumberObject.hpp>
#include <Flosion/Objects/Functions.hpp>
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

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

    class Slider : public NumberObject {
    public:
        Slider(const ui::String& name = "");

        static std::unique_ptr<Slider> parseSlider(const std::vector<std::string>&);

        double getValue() const noexcept;
        double getMinimum() const noexcept;
        double getMaximum() const noexcept;

        void setValue(double);
        void setMinimum(double);
        void setMaximum(double);

        ui::String name() const;
        void setName(const ui::String&);

    private:
        // TODO: shift-click or double-click or button to edit value and limits
        // TODO: easier way to drag

        flo::Constant m_constant;
        ui::Slider<double>* m_slider;
        ui::VerticalList* m_list;
        ui::Text* m_label;
        flo::Connection m_conn;

        void addLabel();

        void removeLabel();

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

    class TrivialNumberObject : public NumberObject {
    private:
        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

    class Add : public TrivialNumberObject {
    public:
        Add();

    private:
        flo::Add m_add;
    };

    class Subtract : public TrivialNumberObject {
    public:
        Subtract();

    private:
        flo::Subtract m_subtract;
    };

    class Multiply : public TrivialNumberObject {
    public:
        Multiply();

    private:
        flo::Multiply m_multiply;
    };

    class Divide : public TrivialNumberObject {
    public:
        Divide();

    private:
        flo::Divide m_divide;
    };

    class PiConstant : public TrivialNumberObject {
    public:
        PiConstant();

    private:
        flo::PiConstant m_piConstant;
    };

    class EulersConstant : public TrivialNumberObject {
    public:
        EulersConstant();

    private:
        flo::EulersConstant m_eulersConstant;
    };

    class TauConstant : public TrivialNumberObject {
    public:
        TauConstant();

    private:
        flo::TauConstant m_tauConstant;
    };

    class SampleFrequencyConstant : public TrivialNumberObject {
    public:
        SampleFrequencyConstant();

    private:
        flo::SampleFrequencyConstant m_sfreq;
    };

    class Abs : public TrivialNumberObject {
    public:
        Abs();

    private:
        flo::Abs m_abs;
    };

    class SquareRoot : public TrivialNumberObject {
    public:
        SquareRoot();

    private:
        flo::SquareRoot m_squareRoot;
    };

    class CubeRoot : public TrivialNumberObject {
    public:
        CubeRoot();

    private:
        flo::CubeRoot m_cubeRoot;
    };

    class Square : public TrivialNumberObject {
    public:
        Square();

    private:
        flo::Square m_square;
    };

    class Log : public TrivialNumberObject {
    public:
        Log();

    private:
        flo::Log m_log;
    };

    class Log2 : public TrivialNumberObject {
    public:
        Log2();

    private:
        flo::Log2 m_log2;
    };

    class Log10 : public TrivialNumberObject {
    public:
        Log10();

    private:
        flo::Log10 m_log10;
    };

    class Exp : public TrivialNumberObject {
    public:
        Exp();

    private:
        flo::Exp m_exp;
    };

    class Exp2 : public TrivialNumberObject {
    public:
        Exp2();

    private:
        flo::Exp2 m_exp2;
    };

    class Exp10 : public TrivialNumberObject {
    public:
        Exp10();

    private:
        flo::Exp10 m_exp10;
    };

    class Sin : public TrivialNumberObject {
    public:
        Sin();

    private:
        flo::Sin m_sin;
    };

    class Cos : public TrivialNumberObject {
    public:
        Cos();

    private:
        flo::Cos m_cos;
    };

    class Tan : public TrivialNumberObject {
    public:
        Tan();

    private:
        flo::Tan m_tan;
    };

    class Asin : public TrivialNumberObject {
    public:
        Asin();

    private:
        flo::Asin m_asin;
    };

    class Acos : public TrivialNumberObject {
    public:
        Acos();

    private:
        flo::Acos m_acos;
    };

    class Atan : public TrivialNumberObject {
    public:
        Atan();

    private:
        flo::Atan m_atan;
    };

    class Sinh : public TrivialNumberObject {
    public:
        Sinh();

    private:
        flo::Sinh m_sinh;
    };

    class Cosh : public TrivialNumberObject {
    public:
        Cosh();

    private:
        flo::Cosh m_cosh;
    };

    class Tanh : public TrivialNumberObject {
    public:
        Tanh();

    private:
        flo::Tanh m_tanh;
    };

    class Asinh : public TrivialNumberObject {
    public:
        Asinh();

    private:
        flo::Asinh m_asinh;
    };

    class Acosh : public TrivialNumberObject {
    public:
        Acosh();

    private:
        flo::Acosh m_acosh;
    };

    class Atanh : public TrivialNumberObject {
    public:
        Atanh();

    private:
        flo::Atanh m_atanh;
    };

    class Ceil : public TrivialNumberObject {
    public:
        Ceil();

    private:
        flo::Ceil m_ceil;
    };

    class Floor : public TrivialNumberObject {
    public:
        Floor();

    private:
        flo::Floor m_floor;
    };

    class Round : public TrivialNumberObject {
    public:
        Round();

    private:
        flo::Round m_round;
    };

    class Frac : public TrivialNumberObject {
    public:
        Frac();

    private:
        flo::Frac m_frac;
    };

    class PlusOne : public TrivialNumberObject {
    public:
        PlusOne();

    private:
        flo::PlusOne m_plusOne;
    };

    class MinusOne : public TrivialNumberObject {
    public:
        MinusOne();

    private:
        flo::MinusOne m_minusOne;
    };

    class OneMinus : public TrivialNumberObject {
    public:
        OneMinus();

    private:
        flo::OneMinus m_oneMinus;
    };
    
    class Negate : public TrivialNumberObject {
    public:
        Negate();

    private:
        flo::Negate m_negate;
    };

    class Reciprocal : public TrivialNumberObject {
    public:
        Reciprocal();

    private:
        flo::Reciprocal m_reciprocal;
    };

    class StdToNorm : public TrivialNumberObject {
    public:
        StdToNorm();

    private:
        flo::StdToNorm m_stdToNorm;
    };

    class NormToStd : public TrivialNumberObject {
    public:
        NormToStd();

    private:
        flo::NormToStd m_normToStd;
    };

    class Sigmoid : public TrivialNumberObject {
    public:
        Sigmoid();

    private:
        flo::Sigmoid m_sigmoid;
    };

    class Min : public TrivialNumberObject {
    public:
        Min();

    private:
        flo::Min m_min;
    };

    class Max : public TrivialNumberObject {
    public:
        Max();

    private:
        flo::Max m_max;
    };

    class Pow : public TrivialNumberObject {
    public:
        Pow();

    private:
        flo::Pow m_pow;
    };

    class LogBase : public TrivialNumberObject {
    public:
        LogBase();

    private:
        flo::LogBase m_logBase;
    };

    class Hypot : public TrivialNumberObject {
    public:
        Hypot();

    private:
        flo::Hypot m_hypot;
    };

    class Atan2 : public TrivialNumberObject {
    public:
        Atan2();

    private:
        flo::Atan2 m_atan2;
    };

    class RandomUniform : public TrivialNumberObject {
    public:
        RandomUniform();

    private:
        flo::RandomUniform m_randomUniform;
    };

    class RandomNormal : public TrivialNumberObject {
    public:
        RandomNormal();

    private:
        flo::RandomNormal m_randomNormal;
    };

    class RoundTo : public TrivialNumberObject {
    public:
        RoundTo();

    private:
        flo::RoundTo m_roundTo;
    };

    class FloorTo : public TrivialNumberObject {
    public:
        FloorTo();

    private:
        flo::FloorTo m_floorTo;
    };

    class CeilTo : public TrivialNumberObject {
    public:
        CeilTo();

    private:
        flo::CeilTo m_ceilTo;
    };

    class Remainder : public TrivialNumberObject {
    public:
        Remainder();

    private:
        flo::Remainder m_remainder;
    };

    class Gaussian : public TrivialNumberObject {
    public:
        Gaussian();

    private:
        flo::Gaussian m_gaussian;
    };

    class LinearInterpolation : public TrivialNumberObject {
    public:
        LinearInterpolation();

    private:
        flo::LinearInterpolation m_linearInterpolation;
    };

} // namespace flui
