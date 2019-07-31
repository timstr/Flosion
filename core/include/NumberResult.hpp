#pragma once

#include <NumberNode.hpp>

namespace flo {

    class NumberResult : public flo::NumberNode {

    private:
        // TODO: a NumberResult SHOULD return a soundnode that is
        // inaccessible everywhere else. This way, only global/stateless
        // number sources may be connected to it.
        const SoundNode* getStateOwner() const noexcept override final;
    };

} // namespace flo
