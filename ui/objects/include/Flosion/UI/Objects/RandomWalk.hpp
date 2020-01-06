#pragma once

#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/Objects/RandomWalk.hpp>

namespace flui {

    class RandomWalk : public BorrowingNumberObject {
    public:
        RandomWalk();

    private:
        flo::RandomWalk m_randomWalk;
    };

} // namespace flui
