#pragma once

#include <NumberSource.hpp>

namespace flo::number {

    class NumberInput {
        
        double getValue(const sound::SoundState* context) const noexcept;

        // TODO

    private:
        NumberSource* m_source;
    };

} // namespace flo::number
