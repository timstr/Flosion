#pragma once

#include <NumberSource.hpp>
#include <StateBorrower.hpp>

namespace flo::number {

    class StatefulNumberSource : public NumberSource, public StateBorrower {
    public:
        StatefulNumberSource(EvaluationFunction) noexcept;
        // TODO
    };

} //namespace flo::number