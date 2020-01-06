#pragma once

#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/Objects/ExponentialSmoother.hpp>

namespace flui {

    class ExponentialSmoother : public BorrowingNumberObject {
    public:
        ExponentialSmoother();

    private:
        flo::ExponentialSmoother m_smoother;
    };

} // namespace flui
