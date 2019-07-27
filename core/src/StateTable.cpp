#include <StateTable.hpp>
#include <StateAllocator.hpp>
#include <SoundNode.hpp>

#include <algorithm>
#include <cassert>

namespace flo {

    StateTable::StateTable(SoundNode* owner)
        : m_owner(owner)
        , m_numDependentStates(0)
        , m_numKeys(0)
        , m_slotSize(-1)
        , m_data(nullptr) {

    }

    StateTable::~StateTable(){
        if (!m_data){
            return;
        }

        for (size_t i = 0, iEnd = m_numDependentStates * m_numKeys; i != iEnd; ++i){
            destroySlot(m_data + (i * m_slotSize));
        }

        deallocateData(m_data);
    }

    size_t StateTable::getDependentOffset(const SoundNode * dependent) const noexcept {
        auto it = std::find_if(m_dependentOffsets.begin(), m_dependentOffsets.end(), [&](const DependentOffset& d){ return d.dependent == dependent; });
        assert(it != m_dependentOffsets.end());
        return it->offset;
    }

    StateAllocator* StateTable::getMainAllocator(){
        if (!m_mainAllocator){
            assert(m_numKeys == 0);
            assert(m_numDependentStates == 0);
            assert(m_slotSize == static_cast<size_t>(-1));
            assert(m_slotItems.size() == 0);
            m_mainAllocator = makeAllocator();
            m_slotSize = m_mainAllocator->getSize();
        }
        return m_mainAllocator.get();
    }

    unsigned char* StateTable::allocateData(size_t slotSize, size_t numSlots){
        const auto align = std::align_val_t{getMainAllocator()->getAlignment()};
        void* ptr = operator new(slotSize * numSlots, align);
        return static_cast<unsigned char*>(ptr);
    }

    void StateTable::deallocateData(unsigned char* ptr){
        const auto align = std::align_val_t{getMainAllocator()->getAlignment()};
        operator delete(static_cast<void*>(ptr), align);
    }

    void StateTable::constructSlot(unsigned char* where, const flo::SoundState* dependentState){
        getMainAllocator()->construct(where, m_owner, dependentState);
        for (auto& slot : m_slotItems){
            slot.allocator->construct(where + slot.offset, m_owner, dependentState);
        }
    }

    void StateTable::destroySlot(unsigned char* where){
        getMainAllocator()->destroy(where);
        for (auto& slot : m_slotItems){
            slot.allocator->destroy(where + slot.offset);
        }
    }

    void StateTable::moveSlot(unsigned char* from, unsigned char* to, const SoundState* newDependentState){
        getMainAllocator()->moveConstruct(to, from);
        (reinterpret_cast<SoundState*>(to))->m_dependentState = newDependentState;
        for (auto& slot : m_slotItems){
            slot.allocator->moveConstruct(to + slot.offset, from + slot.offset);
        }
        destroySlot(from);
    }

    void StateTable::resetSlot(unsigned char* where){
        reinterpret_cast<SoundState*>(where)->reset();
        for (auto& slot : m_slotItems){
            reinterpret_cast<State*>(where + slot.offset)->reset();
        }
    }

    void StateTable::repointStatesFor(const SoundNode* dependent) noexcept {
        const auto dsBegin = getDependentOffset(dependent);
        const auto dsEnd = dsBegin + dependent->numSlots();
        for (size_t i = dsBegin; i < dsEnd; ++i){
            const auto dependentState = dependent->getState(i);
            for (size_t j = 0; j < numKeys(); ++j){
                getState((i * numKeys()) + j)->m_dependentState = dependentState;
            }
        }
    }

    SoundState* StateTable::getState(const SoundNode* dependent, const SoundState* dependentState, size_t keyIndex) noexcept {
        return const_cast<SoundState*>(const_cast<const StateTable*>(this)->getState(dependent, dependentState, keyIndex));
    }

    const SoundState* StateTable::getState(const SoundNode* dependent, const SoundState* dependentState, size_t keyIndex) const noexcept {
        const auto offset = getDependentOffset(dependent);
        return getState(offset + (dependent->getStateIndex(dependentState) * numKeys()) + keyIndex);
    }

    SoundState* StateTable::getState(size_t slotIndex) noexcept {
        return const_cast<SoundState*>(const_cast<const StateTable*>(this)->getState(slotIndex));
    }

    const SoundState* StateTable::getState(size_t slotIndex) const noexcept {
        assert(slotIndex < numSlots());
        return reinterpret_cast<const SoundState*>(m_data + (slotIndex * m_slotSize));
    }

    void StateTable::resetStateFor(const SoundNode* dependent, const SoundState* dependentState) noexcept {
        const auto stateIdx = dependent->getStateIndex(dependentState);


        const auto baseSlot = getDependentOffset(dependent) + (stateIdx * numKeys());
        for (size_t j = 0; j < numKeys(); ++j){
            const auto slotIdx = baseSlot + j;
            const auto slotAddr = m_data + (m_slotSize * slotIdx);
            resetSlot(slotAddr);
            for (const auto& d : m_owner->getDirectDependents()){
                d->resetStateFor(m_owner, reinterpret_cast<const SoundState*>(slotAddr));
            }
        }
    }

    size_t StateTable::getStateIndex(const SoundState* ownState) const noexcept {
        auto idx = (reinterpret_cast<const unsigned char*>(ownState) - m_data) / m_slotSize;
        assert(idx < numSlots());
        return static_cast<size_t>(idx);
    }

    size_t StateTable::numSlots() const noexcept {
        return numDependentStates() * numKeys();
    }

    size_t StateTable::numDependentStates() const noexcept {
        return m_numDependentStates;
    }

    size_t StateTable::numKeys() const noexcept {
        return m_numKeys;
    }

    void StateTable::insertDependentStates(const SoundNode* dependent, size_t beginIndex, size_t endIndex){
        assert(
            std::find_if(
                m_dependentOffsets.begin(),
                m_dependentOffsets.end(),
                [&](const DependentOffset& d){ return d.dependent == dependent; }
            ) != m_dependentOffsets.end()
        );
        assert(beginIndex <= endIndex);
        assert(beginIndex < dependent->numSlots());
        assert(endIndex <= dependent->numSlots());

        if (beginIndex == endIndex){
            return;
        }

        // allocate new array
        auto newNumSlots = numKeys() * (numDependentStates() + (endIndex - beginIndex));
        auto oldData = m_data;
        auto newData = allocateData(m_slotSize, newNumSlots);

        // indices of next slots to be moved/added
        auto oldSlotIndex = size_t{0};
        auto newSlotIndex = size_t{0};

        auto itDependent = m_dependentOffsets.begin();
        assert(itDependent != m_dependentOffsets.end());

        // for all dependents prior to the given dependent
        while (itDependent->dependent != dependent){
            // move all states to new array
            for (size_t i = 0; i < itDependent->dependent->numSlots(); i++){
                for (size_t j = 0; j < numKeys(); ++j){
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to, itDependent->dependent->getState(i));

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }
            ++itDependent;
            assert(itDependent != m_dependentOffsets.end());
        }

        assert(itDependent != m_dependentOffsets.end());
        assert(itDependent->dependent == dependent);

        // move states for the given dependent with new states (this is the interesting part)
        {
            // move all the dependent's states prior to the first new state
            for (size_t i = 0; i < beginIndex; ++i){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex == (itDependent->offset + i));
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to, dependent->getState(i));

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }

            // move all the dependent's new states
            for (size_t i = beginIndex; i < endIndex; ++i){
                for (size_t j = 0; j < numKeys(); ++j){
                    auto dst = newData + (m_slotSize * newSlotIndex);
                    constructSlot(dst, dependent->getState(i));

                    ++newSlotIndex;
                }
            }

            // move all the depdendent's states after the last new state
            for (size_t i = endIndex, iEnd = dependent->numSlots(); i < iEnd; ++i){
            for (size_t j = 0; j < numKeys(); ++j){
                assert(oldSlotIndex == itDependent->offset + i - (endIndex - beginIndex));
                auto from = oldData + (m_slotSize * oldSlotIndex);
                auto to = newData + (m_slotSize * newSlotIndex);
                moveSlot(from, to, dependent->getState(i));

                ++oldSlotIndex;
                ++newSlotIndex;
            }
        }
        }

        // move the states of all remaining dependents
        ++itDependent;
        while (itDependent != m_dependentOffsets.end()){
            assert(itDependent->dependent != dependent);
            itDependent->offset = newSlotIndex;
            for (size_t i = 0; i < itDependent->dependent->numSlots(); i++){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex == itDependent->offset + i);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to, itDependent->dependent->getState(i));

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }
            ++itDependent;
        }

        assert(newSlotIndex == newNumSlots);

        // cleanup
        deallocateData(oldData);
        m_data = newData;

        // propagate the changes
        {
            const auto offset = getDependentOffset(dependent);
            const auto ownStartIndex = numKeys() * (offset + beginIndex);
            const auto ownEndIndex = numKeys() * (offset + endIndex);
            for (auto& d : m_owner->getDirectDependents()){
                d->insertDependentStates(m_owner, ownStartIndex, ownEndIndex);
                d->repointStatesFor(m_owner);
            }
        }
    }

    void StateTable::eraseDependentStates(const SoundNode* dependent, size_t beginIndex, size_t endIndex){
        assert(
            std::find_if(
                m_dependentOffsets.begin(),
                m_dependentOffsets.end(),
                [&](const DependentOffset& d){ return d.dependent == dependent; }
            ) != m_dependentOffsets.end()
        );

        assert(beginIndex < endIndex);

        // allocate new array
        auto newNumSlots = numKeys() * (numDependentStates() - (endIndex - beginIndex));
        auto oldData = m_data;
        auto newData = allocateData(m_slotSize, newNumSlots);

        // indices of next slots to be moved/added
        auto oldSlotIndex = size_t{0};
        auto newSlotIndex = size_t{0};

        auto itDependent = m_dependentOffsets.begin();
        assert(itDependent != m_dependentOffsets.end());

        // for all dependents prior to the given dependent
        while (itDependent->dependent != dependent){
            // move all states to new array
            for (size_t i = 0; i < itDependent->dependent->numSlots(); i++){
                for (size_t j = 0; j < numKeys(); ++j){
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to, itDependent->dependent->getState(i));

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }
            ++itDependent;
            assert(itDependent != m_dependentOffsets.end());
        }

        assert(itDependent != m_dependentOffsets.begin());
        assert(itDependent->dependent == dependent);

        // begin moving states for the given dependent with new states

        assert(beginIndex < dependent->numSlots());
        assert(endIndex <= dependent->numSlots());

        // move all the dependent's states prior to the first new state
        for (size_t i = 0; i < beginIndex; ++i){
            for (size_t j = 0; j < numKeys(); ++j){
                assert(oldSlotIndex == (itDependent->offset + i));
                auto from = oldData + (m_slotSize * oldSlotIndex);
                auto to = newData + (m_slotSize * newSlotIndex);
                moveSlot(from, to, dependent->getState(i));

                ++oldSlotIndex;
                ++newSlotIndex;
            }
        }

        // destroy all the dependent's old states
        for (size_t i = beginIndex; i < endIndex; ++i){
            for (size_t j = 0; j < numKeys(); ++j){
                auto dst = oldData + (m_slotSize * oldSlotIndex);
                destroySlot(dst);

                ++oldSlotIndex;
            }
        }

        // move all the depdendent's states after the last state being removed
        for (size_t i = endIndex, iEnd = dependent->numSlots(); i < iEnd; ++i){
            for (size_t j = 0; j < numKeys(); ++j){
                auto from = oldData + (m_slotSize * oldSlotIndex);
                auto to = newData + (m_slotSize * newSlotIndex);
                moveSlot(from, to, dependent->getState(i - (endIndex - beginIndex)));

                ++oldSlotIndex;
                ++newSlotIndex;
            }
        }

        // move the states of all remaining dependents
        ++itDependent;
        while (itDependent != m_dependentOffsets.end()){
            assert(itDependent->dependent != dependent);
            itDependent->offset = newSlotIndex;
            for (size_t i = 0; i < itDependent->dependent->numSlots(); i++){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex == itDependent->offset + i);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to, itDependent->dependent->getState(i));

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }
            ++itDependent;
        }

        assert(newSlotIndex == newNumSlots);

        // cleanup
        deallocateData(oldData);
        m_data = newData;

        // propagate the changes
        {
            const auto offset = getDependentOffset(dependent);
            const auto ownStartIndex = numKeys() * (offset + beginIndex);
            const auto ownEndIndex = numKeys() * (offset + endIndex);
            for (auto& d : m_owner->getDirectDependents()){
                d->eraseDependentStates(m_owner, ownStartIndex, ownEndIndex);
                d->repointStatesFor(m_owner);
            }
        }
    }

} // namespace flo
