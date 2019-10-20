#pragma once

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Functions.hpp>

#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    class Add : public Object {
    public:
        Add();

    private:
        flo::Add m_add;
    };

} // namespace flui
