#pragma once

#include <Flosion/UI/Core/Object.hpp>

namespace flui {

    class NumberObject : public Object {
        // TODO

    private:
        const NumberObject* toNumberObject() const noexcept override final;
    };

} // namespace flui
