#pragma once

#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/Objects/Oscillator.hpp>

namespace flui {

    class Oscillator : public BorrowingNumberObject {
    public:
        Oscillator();

    private:
        flo::Oscillator m_oscillator;
    };

} // namespace flui
