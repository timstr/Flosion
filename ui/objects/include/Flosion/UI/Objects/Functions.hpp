#pragma once

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Functions.hpp>

#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    class Constant : public Object {
    public:
        Constant();

    private:
        flo::Constant m_constant;
    };

    class Slider : public Object {
    public:
        Slider();

    private:
        flo::Constant m_constant;
    };

    class Add : public Object {
    public:
        Add();

    private:
        // TODO: when this is destroyed, it severs its own connections,
        // but the Object's pegs and attached wires remain pointing to
        // this object. This is a problem.
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
