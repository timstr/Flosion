#pragma once

#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/Objects/LinearSmoother.hpp>

namespace flui {

    class LinearSmoother : public BorrowingNumberObject {
    public:
        LinearSmoother();

    private:
        flo::LinearSmoother m_smoother;
    };

} // namespace flui
