#pragma once

#include <SoundState.hpp>
#include <StateAllocator.hpp>

#include <memory>
#include <vector>

namespace flo {

    class StateBorrower;
    class SoundNode;

    /*
     * StateTable is an array-like container of custom SoundState objects.
     * Additional states may be stored alongside the SoundStates to be used by
     * state borrowers such as stateful number sources.
     * Sound states are stored inside slots right beside their associated borrowed
     * states. Slots are stored contiguously.
     * StateTable is the basis for an associative mapping from dependent sound nodes,
     * dependent states, etc, to the states needed by a sound node.
     */


    class StateTable final {
    public:
        StateTable(SoundNode* owner, std::unique_ptr<StateAllocator> mainAllocator);
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

        size_t size() const noexcept;
        size_t numBorrowers() const noexcept;

        void resize(size_t newSize);
        void reserve(size_t numSlots);

        size_t slotSize() const noexcept;

        void insertSlot(size_t slotIndex);
        void eraseSlot(size_t slotIndex);
        void pushSlot();
        void popSlot();

    private:
        SoundNode* m_owner;

        std::unique_ptr<StateAllocator> m_mainAllocator;

        // Per slot item data
        struct SlotItem {

            // The allocator used to manage the lifetime of the slot item
            std::unique_ptr<StateAllocator> allocator;

            // The offset (in bytes) from the slot's begin to the item
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

        unsigned char* allocateData(size_t slotSize, size_t numSlots, size_t alignment);
        void deallocateData(unsigned char*);

        // constructs a slot in place from uninitialized storage
        void constructSlot(unsigned char* where, const flo::SoundState* dependentState);

        // destroys a slot in place
        void destroySlot(unsigned char* where);

        // moves a slot from one location to another
        // the old slot is destroyed
        void moveSlot(unsigned char* from, unsigned char* to);

        // moves a slot from one location to another while adding a slot item
        // the slot item must be present in m_slotItems
        // the old slot is destroyed
        void moveSlotAndAddItem(unsigned char* from, unsigned char* to, const StateAllocator* whichItem);
        
        // moves a slot from one location to another while removing a slot item
        // the old slot is destroyed
        void moveSlotAndRemoveItem(unsigned char* from, unsigned char* to, const StateAllocator* whichItem);
    };

} // namespace flo
