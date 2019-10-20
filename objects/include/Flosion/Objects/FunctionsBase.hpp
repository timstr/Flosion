#pragma once

#include <Flosion/Core/NumberSource.hpp>

namespace flo {

    class UnaryFunction : public flo::NumberSource {
    public:
        UnaryFunction(double defaultValue = 0.0);

        flo::NumberSourceInput input;
    };

    class BinaryFunction : public flo::NumberSource {
    public:
        BinaryFunction(double defaultValue1 = 0.0, double defaultValue2 = 0.0);

        flo::NumberSourceInput input1;
        flo::NumberSourceInput input2;
    };

} // namespace flo
