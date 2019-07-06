#pragma once

#include <SoundState.hpp>

#include <memory>
#include <vector>

namespace flo::sound {

    class StateAllocator;
    class StateBorrower;
    class StateUser;

    template<typename SoundStateType>
    class StateArray {
    public:
        StateArray(const StateUser*);
        StateArray(StateArray&&);
        StateArray& operator=(StateArray&&);
        ~StateArray();

        StateArray(const StateArray&) = delete;
        StateArray& operator=(const StateArray&) = delete;



        void addBorrower(StateBorrower*);
        void removeBorrower(StateBorrower*);

        State& getBorrowedState(size_t soundStateIndex, size_t borrowedStateOffset) noexcept;
        const State& getBorrowedState(size_t soundStateIndex, size_t borrowedStateOffset) const noexcept;

        SoundStateType& getState(size_t soundStateIndex) noexcept;
        const SoundStateType& getState(size_t soundStateIndex) const noexcept;

        size_t numBorrowers() const noexcept;
        size_t numSoundStates() const noexcept;

        void addStatesFor(const std::vector<const SoundState*>&);
        void removeStatesFor(const std::vector<const SoundState*>&);
        void resetState(const SoundState*) noexcept;

    private:
        const StateOwner* m_stateOwner;
        const StateUser* m_stateUser;
        unsigned char* m_data; // NOTE: this needs to be aligned according to requirements of all states
        // see item (4) at https://en.cppreference.com/w/cpp/memory/new/operator_new

        std::vector<std::unique_ptr<StateAllocator>> m_allocators;
        size_t m_numSlots;
        size_t m_slotSize;


    };

} // namespace flo::sound
