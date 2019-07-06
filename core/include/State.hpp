#pragma once

namespace flo {

    // TODO: test performance of using function pointer instead of
    // virtual function

    class State {
    public:
        virtual void reset() noexcept = 0;

    };

} // namespace flo
