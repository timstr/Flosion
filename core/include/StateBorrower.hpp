#pragma once

#include <StateAllocator.hpp>

#include <memory>

namespace flo {

    class SoundNode;

    class StateBorrower {
    public:
        StateBorrower() noexcept;

        virtual std::unique_ptr<StateAllocator> makeAllocater() const = 0;

        SoundNode* getStateLender() noexcept;
        const SoundNode* getStateLender() const noexcept;

        void borrowFrom(SoundNode*);

    private:
        SoundNode* m_stateLender;
        size_t m_stateOffset;

        friend class StateTable;
    };

} // namespace flo
