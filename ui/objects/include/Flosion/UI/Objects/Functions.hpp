#pragma once

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Functions.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <GUI/Helpers/Slider.hpp>


namespace flui {

    class Constant : public Object, private flo::ConstantReactor {
    public:
        Constant();

        static std::unique_ptr<Constant> parseConstant(const std::string&);

        double getValue() const noexcept;
        void setValue(double);

    private:
        void onChangeValue(double) override;

        // TODO: shift-click or double-click or button to edit value
        // TODO: easier way to drag

        flo::Constant m_constant;
        ui::Text* m_label;
    };

    class Slider : public Object, private flo::ConstantReactor {
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


        void onChangeValue(double) override;

        flo::Constant m_constant;
        ui::Slider<double>* m_slider;
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

} // namespace flui
