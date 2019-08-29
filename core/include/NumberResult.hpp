#pragma once

#include <NumberNode.hpp>
#include <SoundResult.hpp>
#include <SoundSource.hpp>

namespace flo {

    class NumberResult : public flo::NumberNode {
    private:
        std::unique_ptr<SoundResult> m_hiddenNode;

    public:
        NumberResult() noexcept;

        double getValue() const noexcept;

        SoundNumberInput input;
    };

} // namespace flo
