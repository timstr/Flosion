#pragma once

#include <Flosion/Core/NumberNode.hpp>
#include <Flosion/Core/SoundResult.hpp>
#include <Flosion/Core/SoundSource.hpp>

namespace flo {

    class NumberResult : public flo::NumberNode {
    public:
        NumberResult() noexcept;

        double getValue() const noexcept;

        SoundNumberInput input;

    private:
        std::unique_ptr<SoundResult> m_hiddenNode;
    };

} // namespace flo
