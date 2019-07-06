#pragma once

#include <StateUser.hpp>
#include <StateAllocator.hpp>

#include <memory>

namespace flo {

    class StateOwner;

    class StateBorrower : public StateUser {
    public:
        virtual std::unique_ptr<StateAllocator> makeAllocater() const = 0;

        // TODO

    private:
        StateOwner* m_stateOwner;
        size_t m_stateOffset;
    };

} // namespace flo
