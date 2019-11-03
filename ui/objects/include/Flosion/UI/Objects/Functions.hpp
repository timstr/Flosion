#pragma once

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Functions.hpp>

#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    class Constant : public Object, private flo::ConstantReactor {
    public:
        Constant();

        static std::unique_ptr<Constant> parseConstant(const std::string&);

        flo::Constant& getConstant() noexcept;

    private:
        void onChangeValue(double) override;

        // TODO: shift-click or double-click to edit value

        flo::Constant m_constant;
        ui::Text* m_label;
    };

    // TODO
    class Slider : public Object, private flo::ConstantReactor {
    public:
        Slider();

    private:
        flo::Constant m_constant;
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
