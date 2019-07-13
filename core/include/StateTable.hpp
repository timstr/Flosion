#pragma once

#include <SoundState.hpp>
#include <SoundNode.hpp>

#include <memory>
#include <vector>

namespace flo {

    class StatefulSoundNode;
    class StateAllocator;
    class StateBorrower;

    /*
     * StateTable is an array-like container of custom SoundState objects.
     * Additional states may be stored alongside the SoundStates to be used by
     * state borrowers such as stateful number sources.
     * Sound states are stored inside slots right beside their associated borrowed
     * states. Slots are stored contiguously.
     * StateTable is the basis for an associative mapping from dependent sound nodes,
     * dependent states, etc, to the states needed by a sound node.
     */
    class StateTable : virtual public SoundNode {
    public:
        StateTable(SoundNetwork* network, SoundNode::Type type, std::unique_ptr<StateAllocator> mainAllocator);
        StateTable(StateTable&&) noexcept = default;
        StateTable& operator=(StateTable&&) noexcept = default;
        ~StateTable();

        StateTable(const StateTable&) = delete;
        StateTable& operator=(const StateTable&) = delete;

        void addBorrower(StateBorrower*);
        void removeBorrower(StateBorrower*);

        SoundState* getState(size_t slotIndex) noexcept;
        const SoundState* getState(size_t slotIndex) const noexcept;

        State* getBorrowedState(const SoundState* mainState, const StateBorrower* borrower) noexcept;
        const State* getBorrowedState(const SoundState* mainState, const StateBorrower* borrower) const noexcept;

        size_t getStateIndex(const SoundState* ownState) const noexcept;

        size_t numSlots() const noexcept;
        size_t numBorrowers() const noexcept;

        void resize(size_t newSize);
        void reserve(size_t numSlots);

        size_t slotSize() const noexcept;

        void insertSlot(size_t slotIndex);
        void eraseSlot(size_t slotIndex);
        void pushSlot();
        void popSlot();

    private:
        std::unique_ptr<StateAllocator> m_mainAllocator;

        struct SlotItem {
            std::unique_ptr<StateAllocator> allocator;
            size_t offset;
        };

        std::vector<SlotItem> m_slotItems;

        unsigned char* m_data; // NOTE: this needs to be aligned according to requirements of all states
        // see item (4) at https://en.cppreference.com/w/cpp/memory/new/operator_new

        // See also: https://en.cppreference.com/w/cpp/memory/align
        // for computing successive addresses

        // number of bytes per slot, including padding to account for alignment
        size_t m_slotSize;

        // number of active slots
        size_t m_numSlots;

        // number of slots for which there is available storage
        size_t m_capacity;

        StateTable* toStateTable() noexcept override;

        unsigned char* allocateData(size_t slotSize, size_t numSlots, size_t alignment);
        void deallocateData(unsigned char*);

        void constructSlot(unsigned char* where);
        void destroySlot(unsigned char* where);
        void moveSlot(unsigned char* from, unsigned char* to);
        void moveSlotAndAddSlotItem(unsigned char* from, unsigned char* to, const StateAllocator* which);
        void moveSlotAndRemoveSlotItem(unsigned char* from, unsigned char* to, const StateAllocator* which);
    };

} // namespace flo
