#include <Flosion/Core/StateTable.hpp>

#include <Flosion/Core/StateAllocator.hpp>
#include <Flosion/Core/SoundNode.hpp>
#include <Flosion/Core/BorrowingNumberSource.hpp>

#include <algorithm>
#include <cassert>
#include <numeric>

namespace flo {

    StateTable::StateTable(SoundNode* owner)
        : m_owner(owner)
        , m_numDependentStates(0)
        , m_numKeys(0)
        , m_slotSize(static_cast<std::size_t>(-1))
        , m_isMonostate(false)
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

    size_t StateTable::getDependentOffset(const SoundNode* dependent) const noexcept {
        // Please excuse this massive check for invariants via immediately-invoked boolean lambda
        assert([&](){
            // Cumulative sum of expected offsets into state table.
            size_t sum = 0;
            for (const auto& dto : m_dependentOffsets){
                // The offset must match the expected offset
                if (sum != dto.offset){
                    return false;
                }
                if (numKeys() == 0){
                    // If there are zero keys, the offset must also be zero
                    // because the state table is empty
                    if (dto.offset != 0){
                        return false;
                    }
                } else {
                    // If there are keys, the offset must divide the number of keys
                    if (dto.offset % numKeys() != 0){
                        return false;
                    }
                }
                // The next expected offset the current offset plus the number of
                // dependent states times the number of keys
                sum += dto.count * numKeys();
            }
            // The last offset must equal the total number of state slots
            if (sum != numSlots()){
                return false;
            }
            return true;
        }());

        auto it = std::find_if(
            m_dependentOffsets.begin(),
            m_dependentOffsets.end(),
            [&](const DependentOffset& d){
                return d.dependent == dependent;
            }
        );
        assert(it != m_dependentOffsets.end());
        return it->offset;
    }

    size_t StateTable::nextAlignedOffset(size_t minOffset, size_t align) const {
        // make sure that align is a power of 2
        assert(align != 0 && (align & (align - 1)) == 0);
        auto prev = (minOffset / align) * align;
        assert(prev % align == 0);
        assert(prev <= minOffset);
        assert(prev + align > minOffset);
        return (prev == minOffset) ? prev : prev + align;
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
        assert(m_mainAllocator);
        return m_mainAllocator.get();
    }

    unsigned char* StateTable::allocateData(size_t slotSize, size_t numSlots){
        const auto align = std::align_val_t{getMainAllocator()->getAlignment()};
        void* ptr = operator new(slotSize * numSlots, align);
        return static_cast<unsigned char*>(ptr);
    }

    void StateTable::deallocateData(unsigned char* ptr){
        if (ptr == nullptr){
            return;
        }
        const auto align = std::align_val_t{getMainAllocator()->getAlignment()};
        operator delete(static_cast<void*>(ptr), align);
    }

    void StateTable::constructSlot(unsigned char* where){
        getMainAllocator()->construct(where, m_owner, nullptr);
        for (auto& slot : m_slotItems){
            assert(slot.offset != static_cast<size_t>(-1));
            slot.allocator->construct(where + slot.offset, m_owner, nullptr);
        }
    }

    void StateTable::destroySlot(unsigned char* where){
        getMainAllocator()->destroy(where);
        for (auto& slot : m_slotItems){
            assert(slot.offset != static_cast<size_t>(-1));
            slot.allocator->destroy(where + slot.offset);
        }
    }

    void StateTable::moveSlot(unsigned char* from, unsigned char* to){
        getMainAllocator()->moveConstruct(to, from);
        for (auto& slot : m_slotItems){
            assert(slot.offset != static_cast<size_t>(-1));
            slot.allocator->moveConstruct(to + slot.offset, from + slot.offset);
        }
        destroySlot(from);
    }

    void StateTable::resetSlot(unsigned char* where){
        auto ss = reinterpret_cast<SoundState*>(where);
        ss->reset();
        ss->resetTime();
        for (auto& slot : m_slotItems){
            assert(slot.offset != static_cast<size_t>(-1));
            reinterpret_cast<State*>(where + slot.offset)->reset();
        }
    }

    void StateTable::moveSlotAndAddItem(unsigned char* from, unsigned char* to, const BorrowingNumberSource* whichItem){
        getMainAllocator()->moveConstruct(to, from);
        for (auto& slot : m_slotItems){
            assert(slot.offset != static_cast<size_t>(-1));
            if (slot.borrower == whichItem){
                const auto mainState = (reinterpret_cast<SoundState*>(to));
                slot.allocator->construct(to + slot.offset, m_owner, mainState->getDependentState());
            } else {
                assert(slot.previousOffset != static_cast<size_t>(-1));
                slot.allocator->moveConstruct(to + slot.offset, from + slot.previousOffset);
                slot.allocator->destroy(from + slot.previousOffset);
            }
        }
    }

    void StateTable::moveSlotAndRemoveItem(unsigned char* from, unsigned char* to, const BorrowingNumberSource* whichItem) {
        getMainAllocator()->moveConstruct(to, from);
        for (auto& slot : m_slotItems){
            assert(slot.offset != static_cast<size_t>(-1));
            if (slot.borrower == whichItem){
                slot.allocator->destroy(from + slot.offset);
            } else {
                assert(slot.previousOffset != static_cast<size_t>(-1));
                slot.allocator->moveConstruct(to + slot.offset, from + slot.previousOffset);
                slot.allocator->destroy(from + slot.previousOffset);
            }
        }
    }

    void StateTable::repointStatesFor(const SoundNode* dependent) noexcept {
        for (const auto& dto : m_dependentOffsets){
            if (dto.dependent != dependent){
                continue;
            }
            if (numKeys() == 0){
                return;
            }
            for (size_t i = 0; i < dto.count; ++i){
                const auto dependentState = dependent->getState(i);
                for (size_t k = 0; k < numKeys(); ++k){
                    getState(dto.offset + (i * numKeys()) + k)->m_dependentState = dependentState;
                }
            }
            return;
        }
        assert(false);
    }

    void StateTable::addDependentOffset(const SoundNode* d){
        assert(std::find_if(
            m_dependentOffsets.begin(),
            m_dependentOffsets.end(),
            [&](const DependentOffset& x){
                return x.dependent == d;
            }
        ) == m_dependentOffsets.end());
        assert(!m_isMonostate || m_dependentOffsets.size() == 0);

        const auto offset = m_numKeys * std::accumulate(
            m_dependentOffsets.begin(),
            m_dependentOffsets.end(),
            0ull,
            [](size_t acc, const DependentOffset& x){
                return acc + x.count;
            }
        );

        m_dependentOffsets.push_back({
            d,
            offset,
            (m_isMonostate ? 1u : 0u)
        });
    }

    void StateTable::removeDependentOffset(const SoundNode* d){
        assert(!m_isMonostate || (m_dependentOffsets.size() == 1));
        auto it = std::find_if(
            m_dependentOffsets.begin(),
            m_dependentOffsets.end(),
            [&](const DependentOffset& x){
                return x.dependent == d;
            }
        );
        assert(it != m_dependentOffsets.end());
        m_dependentOffsets.erase(it);
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

    State* StateTable::getBorrowedState(const SoundState* mainState, const BorrowingNumberSource* borrower) const noexcept {
        assert(hasState(mainState));
        const auto addr = (reinterpret_cast<const unsigned char*>(mainState) + borrower->m_stateOffset);
        return reinterpret_cast<State*>(const_cast<unsigned char*>(addr));
    }

    const SoundState* StateTable::getMainState(const State* borrowedState) const noexcept {
        const auto idx = ((reinterpret_cast<const unsigned char*>(borrowedState)) - m_data) / m_slotSize;
        return getState(idx);
    }

    void StateTable::resetState(SoundState* ownState){
        assert(hasState(ownState));
        const auto slotIdx = getStateIndex(ownState);
        const auto slotAddr = m_data + (m_slotSize * slotIdx);
        resetSlot(slotAddr);
        for (const auto& d : m_owner->getDirectDependencies()){
            d->resetStateFor(m_owner, ownState);
        }
    }

    void StateTable::resetStateFor(const SoundNode* dependent, const SoundState* dependentState) noexcept {
        const auto stateIdx = dependent->getStateIndex(dependentState);

        const auto baseSlot = getDependentOffset(dependent) + (stateIdx * numKeys());
        for (size_t j = 0; j < numKeys(); ++j){
            const auto slotIdx = baseSlot + j;
            const auto slotAddr = m_data + (m_slotSize * slotIdx);
            const auto slotState = reinterpret_cast<const SoundState*>(slotAddr);
            resetSlot(slotAddr);
            for (const auto& d : m_owner->getDirectDependencies()){
                d->resetStateFor(m_owner, slotState);
            }
        }
    }

    void StateTable::resetStateFor(const SoundNode* dependent, const SoundState* dependentState, size_t keyIndex) noexcept {
        assert(keyIndex < numKeys());
        const auto stateIdx = dependent->getStateIndex(dependentState);

        const auto baseSlot = getDependentOffset(dependent) + (stateIdx * numKeys());

        const auto slotIdx = baseSlot + keyIndex;
        const auto slotAddr = m_data + (m_slotSize * slotIdx);
        const auto slotState = reinterpret_cast<const SoundState*>(slotAddr);
        resetSlot(slotAddr);
        for (const auto& d : m_owner->getDirectDependencies()){
            d->resetStateFor(m_owner, slotState);
        }
    }

    size_t StateTable::getStateIndex(const SoundState* ownState) const noexcept {
        assert(ownState->getOwner() == this);
        assert(hasState(ownState));
        auto idx = (reinterpret_cast<const unsigned char*>(ownState) - m_data) / m_slotSize;
        assert(idx < numSlots());
        return static_cast<size_t>(idx);
    }

    bool StateTable::hasState(const SoundState* ownState) const noexcept {
        const auto addr = reinterpret_cast<const unsigned char*>(ownState);
        const auto diff = addr - m_data;
        if (diff % m_slotSize != 0){
            return false;
        }
        const auto idx = diff / m_slotSize;
        const bool isOwn = (idx >= 0) && (idx < numSlots());
        assert(isOwn == (ownState->getOwner() == this));
        return isOwn;
    }

    size_t StateTable::numSlots() const noexcept {
        return numDependentStates() * numKeys();
    }

    size_t StateTable::numDependentStates() const noexcept {
        assert(!m_isMonostate || (m_numDependentStates == 1));
        return m_numDependentStates;
    }

    size_t StateTable::numKeys() const noexcept {
        assert(!m_isMonostate || (m_numKeys == 1));
        return m_numKeys;
    }

    size_t StateTable::numBorrowers() const noexcept {
        return m_slotItems.size();
    }

    size_t StateTable::slotSize() const noexcept {
        return m_slotSize;
    }

    void StateTable::insertDependentStates(const SoundNode* dependent, size_t beginIndex, size_t endIndex){
        assert(std::find_if(
            m_dependentOffsets.begin(),
            m_dependentOffsets.end(),
            [&](const DependentOffset& d){ return d.dependent == dependent; }
        ) != m_dependentOffsets.end());
        assert(beginIndex <= endIndex);
        assert(beginIndex < dependent->numSlots());
        assert(endIndex <= dependent->numSlots());

        if (beginIndex == endIndex){
            return;
        }

        if (m_isMonostate){
            assert(m_dependentOffsets.size() == 1);
            assert(m_dependentOffsets[0].dependent == dependent);
            assert(m_dependentOffsets[0].offset == 0);
            assert(m_dependentOffsets[0].count == 1);
            assert(beginIndex == 0);
            assert(beginIndex == 1);
            assert(numSlots() == 1);
            assert(getState(0)->m_dependentState == nullptr);
            getState(0)->m_dependentState = dependent->getState(0);
            return;
        }

        if (numKeys() == 0){
            assert(std::all_of(
                m_dependentOffsets.begin(),
                m_dependentOffsets.end(),
                [](const DependentOffset& d){
                    return d.offset == 0;
                }
            ));
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
            assert(itDependent->dependent->numSlots() == itDependent->count);
            for (size_t i = 0; i < itDependent->count; i++){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex < numSlots());
                    assert(newSlotIndex < newNumSlots);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to);

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
                    //assert(oldSlotIndex == (itDependent->offset + i));
                    assert(oldSlotIndex < numSlots());
                    assert(newSlotIndex < newNumSlots);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to);

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }

            // move all the dependent's new states
            for (size_t i = beginIndex; i < endIndex; ++i){
                for (size_t j = 0; j < numKeys(); ++j){
                    auto dst = newData + (m_slotSize * newSlotIndex);
                    assert(newSlotIndex < newNumSlots);
                    constructSlot(dst);

                    ++newSlotIndex;
                }
            }

            // move all the depdendent's states after the last new state
            for (size_t i = endIndex, iEnd = itDependent->count; i < iEnd; ++i){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex == itDependent->offset + i - (endIndex - beginIndex));
                    assert(oldSlotIndex < numSlots());
                    assert(newSlotIndex < newNumSlots);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to);

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }

            // update the number of states
            itDependent->count += (endIndex - beginIndex);
        }

        // move the states of all remaining dependents
        ++itDependent;
        while (itDependent != m_dependentOffsets.end()){
            assert(itDependent->dependent != dependent);
            itDependent->offset = newSlotIndex;
            assert(itDependent->dependent->numSlots() == itDependent->count);
            for (size_t i = 0; i < itDependent->count; i++){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex == itDependent->offset + i);
                    assert(oldSlotIndex < numSlots());
                    assert(newSlotIndex < newNumSlots);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to);

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
        m_numDependentStates += endIndex - beginIndex;

        // propagate the changes
        {
            const auto offset = getDependentOffset(dependent);
            const auto ownStartIndex = offset + (numKeys() * beginIndex);
            const auto ownEndIndex = offset + (numKeys() * endIndex);
            for (auto& d : m_owner->getDirectDependencies()){
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

        if (m_isMonostate){
            assert(m_dependentOffsets.size() == 1);
            assert(m_dependentOffsets[0].dependent == dependent);
            assert(m_dependentOffsets[0].offset == 0);
            assert(m_dependentOffsets[0].count == 1);
            assert(beginIndex == 0);
            assert(beginIndex == 1);
            assert(numSlots() == 1);
            assert(getState(0)->m_dependentState == dependent->getState(0));
            getState(0)->m_dependentState = nullptr;
            m_dependentOffsets[0].count = 0;
            return;
        }

        if (numKeys() == 0){
            assert(std::all_of(
                m_dependentOffsets.begin(),
                m_dependentOffsets.end(),
                [](const DependentOffset& d){
                    return d.offset == 0;
                }
            ));
            return;
        }

        assert(beginIndex < endIndex);
        assert(beginIndex < m_numDependentStates);
        assert(endIndex <= m_numDependentStates);
        assert(m_numDependentStates >= endIndex - beginIndex);

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
            assert(itDependent->dependent->numSlots() == itDependent->count);
            for (size_t i = 0; i < itDependent->count; i++){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex / numKeys() == itDependent->offset + i);
                    assert(oldSlotIndex % numKeys() == j);
                    assert(newSlotIndex / numKeys() == itDependent->offset + i);
                    assert(newSlotIndex % numKeys() == j);
                    
                    assert(oldSlotIndex < numSlots());
                    assert(newSlotIndex < newNumSlots);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to);

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }
            ++itDependent;
            assert(itDependent != m_dependentOffsets.end());
        }

        assert(itDependent != m_dependentOffsets.end());
        assert(itDependent->dependent == dependent);

        // begin moving states for the given dependent with new states
        {
            // move all the dependent's states prior to the first new state
            for (size_t i = 0; i < beginIndex; ++i){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex / numKeys() == itDependent->offset + i);
                    assert(oldSlotIndex % numKeys() == j);
                    assert(newSlotIndex / numKeys() == itDependent->offset + i);
                    assert(newSlotIndex % numKeys() == j);
                
                    assert(oldSlotIndex < numSlots());
                    assert(newSlotIndex < newNumSlots);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to);

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }

            assert(oldSlotIndex / numKeys() == itDependent->offset + beginIndex);
            assert(newSlotIndex / numKeys() == itDependent->offset + beginIndex);

            // destroy all the dependent's old states
            for (size_t i = beginIndex; i < endIndex; ++i){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex / numKeys() == itDependent->offset + i);
                    assert(oldSlotIndex % numKeys() == j);
                
                    assert(oldSlotIndex < numSlots());
                    auto dst = oldData + (m_slotSize * oldSlotIndex);
                    destroySlot(dst);

                    ++oldSlotIndex;
                }
            }

            assert(oldSlotIndex / numKeys() == itDependent->offset + endIndex);
            assert(newSlotIndex / numKeys() == itDependent->offset + beginIndex);

            // move all the dependent's states after the last state being removed
            for (size_t i = endIndex, iEnd = itDependent->count; i < iEnd; ++i){
                for (size_t j = 0; j < numKeys(); ++j){
                    assert(oldSlotIndex / numKeys() == itDependent->offset + i);
                    assert(oldSlotIndex % numKeys() == j);
                    assert(newSlotIndex / numKeys() == itDependent->offset + i - (endIndex - beginIndex));
                    assert(newSlotIndex % numKeys() == j);
                
                    assert(oldSlotIndex < numSlots());
                    assert(newSlotIndex < newNumSlots);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to);

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }

            // update the number of states for the dependent
            assert(itDependent->count >= (endIndex - beginIndex));
            itDependent->count -= (endIndex - beginIndex);
        }

        // move the states of all remaining dependents
        ++itDependent;
        while (itDependent != m_dependentOffsets.end()){
            assert(itDependent->dependent != dependent);
            const auto newOffset = newSlotIndex;
            assert(newOffset == itDependent->offset - numKeys() * (endIndex - beginIndex));
            assert(itDependent->dependent->numSlots() == itDependent->count);
            for (size_t i = 0, iEnd = itDependent->count; i < iEnd; i++){
                for (size_t j = 0; j < numKeys(); ++j){

                    assert(oldSlotIndex / numKeys() == itDependent->offset + i);
                    assert(oldSlotIndex % numKeys() == j);
                    assert(newSlotIndex / numKeys() == itDependent->offset + i - (endIndex - beginIndex));
                    assert(newSlotIndex / numKeys() == newOffset + i);
                    assert(newSlotIndex % numKeys() == j);
                    
                    assert(oldSlotIndex < numSlots());
                    assert(newSlotIndex < newNumSlots);
                    auto from = oldData + (m_slotSize * oldSlotIndex);
                    auto to = newData + (m_slotSize * newSlotIndex);
                    moveSlot(from, to);

                    ++oldSlotIndex;
                    ++newSlotIndex;
                }
            }
            itDependent->offset = newOffset;
            ++itDependent;
        }

        assert(oldSlotIndex == m_numDependentStates * numKeys());

        assert(newSlotIndex == newNumSlots);

        // cleanup
        deallocateData(oldData);
        m_data = newData;
        m_numDependentStates -= endIndex - beginIndex;

        // propagate the changes
        {
            const auto offset = getDependentOffset(dependent);
            const auto ownStartIndex = offset + (numKeys() * beginIndex);
            const auto ownEndIndex = offset + (numKeys() * endIndex);
            for (auto& d : m_owner->getDirectDependencies()){
                d->eraseDependentStates(m_owner, ownStartIndex, ownEndIndex);
                d->repointStatesFor(m_owner);
            }
        }
    }

    void StateTable::insertKeys(size_t beginIndex, size_t endIndex){
        assert(beginIndex < endIndex);
        assert(beginIndex <= numKeys());

        // allocate new data
        const auto oldData = m_data;
        const auto newNumSlots = numDependentStates() * (numKeys() + (endIndex - beginIndex));
        m_data = allocateData(m_slotSize, newNumSlots);
        const auto oldNumKeys = m_numKeys;
        m_numKeys += endIndex - beginIndex;
        
        auto oldSlotIndex = size_t{0};
        auto newSlotIndex = size_t{0};

        // Update dependent offsets
        {
            size_t sum = 0;
            for (auto& dto : m_dependentOffsets){
                dto.offset = sum;
                sum += dto.count * m_numKeys;
            }
            assert(sum == numSlots());
        }

        auto currentDependent = m_dependentOffsets.begin();
        for (size_t i = 0; i < numDependentStates(); ++i){
            assert(currentDependent != m_dependentOffsets.end());
            while ((currentDependent->offset + (currentDependent->count * m_numKeys)) < (i * m_numKeys)){
                ++currentDependent;
                assert(currentDependent != m_dependentOffsets.end());
            }

            // move states before new keys
            for (size_t j = 0; j < beginIndex; ++j){
                assert(oldSlotIndex % oldNumKeys == j);
                assert(newSlotIndex % m_numKeys == j);
                moveSlot(oldData + m_slotSize * oldSlotIndex, m_data + m_slotSize * newSlotIndex);
                ++oldSlotIndex;
                ++newSlotIndex;
            }

            // add states for new keys
            for (size_t j = beginIndex; j < endIndex; ++j){
                assert(newSlotIndex % m_numKeys == j);
                constructSlot(m_data + m_slotSize * newSlotIndex);
                ++newSlotIndex;
            }
            
            // move states after new keys
            for (size_t j = beginIndex; j < oldNumKeys; ++j){
                assert(oldSlotIndex % oldNumKeys == j);
                assert(newSlotIndex % m_numKeys == j + beginIndex - endIndex);
                moveSlot(oldData + m_slotSize * oldSlotIndex, m_data + m_slotSize * newSlotIndex);
                ++oldSlotIndex;
                ++newSlotIndex;
            }

            // propagate changes
            for (auto& d : m_owner->getDirectDependencies()){
                d->insertDependentStates(m_owner, m_numKeys * i + beginIndex, m_numKeys * i + endIndex);
            }

            assert(oldSlotIndex == i * oldNumKeys);
            assert(newSlotIndex == i * m_numKeys);
        }

        for (auto& d : m_owner->getDirectDependencies()){
            d->repointStatesFor(m_owner);
        }

        // clean up
        deallocateData(oldData);
    }

    void StateTable::eraseKeys(size_t beginIndex, size_t endIndex) {
        assert(beginIndex < endIndex);
        assert(endIndex <= numKeys());

        // allocate new data
        const auto oldData = m_data;
        const auto newNumSlots = numDependentStates() * (numKeys() - (endIndex - beginIndex));
        m_data = allocateData(m_slotSize, newNumSlots);
        const auto oldNumKeys = m_numKeys;
        m_numKeys -= endIndex - beginIndex;
        
        auto oldSlotIndex = size_t{0};
        auto newSlotIndex = size_t{0};

        // Update dependent offsets
        {
            size_t sum = 0;
            for (auto& dto : m_dependentOffsets){
                dto.offset = sum;
                sum += dto.count * m_numKeys;
            }
            assert(sum == numSlots());
        }

        auto currentDependent = m_dependentOffsets.begin();
        for (size_t i = 0; i < numDependentStates(); ++i){
            assert(oldSlotIndex == i * oldNumKeys);
            assert(newSlotIndex == i * m_numKeys);

            assert(currentDependent != m_dependentOffsets.end());
            while ((currentDependent->offset + (currentDependent->count * m_numKeys)) < (i * m_numKeys)){
                ++currentDependent;
                assert(currentDependent != m_dependentOffsets.end());
            }

            // move states before new keys
            for (size_t j = 0; j < beginIndex; ++j){
                assert(oldSlotIndex % oldNumKeys == j);
                assert(newSlotIndex % m_numKeys == j);
                moveSlot(oldData + m_slotSize * oldSlotIndex, m_data + m_slotSize * newSlotIndex);
                ++oldSlotIndex;
                ++newSlotIndex;
            }

            // destroy slots of old keys
            for (size_t j = beginIndex; j < endIndex; ++j){
                assert(oldSlotIndex % oldNumKeys == j);
                destroySlot(oldData + m_slotSize * oldSlotIndex);
                ++oldSlotIndex;
            }
            
            // move states after new keys
            for (size_t j = beginIndex; j < m_numKeys; ++j){
                assert(oldSlotIndex % oldNumKeys == j + (endIndex - beginIndex));
                assert(newSlotIndex % m_numKeys == j);
                moveSlot(oldData + m_slotSize * oldSlotIndex, m_data + m_slotSize * newSlotIndex);
                ++oldSlotIndex;
                ++newSlotIndex;
            }

            // propagate changes
            for (auto& d : m_owner->getDirectDependencies()){
                d->eraseDependentStates(m_owner, m_numKeys * i + beginIndex, m_numKeys * i + endIndex);
            }

            assert(oldSlotIndex == (i + 1) * oldNumKeys);
            assert(newSlotIndex == (i + 1) * m_numKeys);
        }

        for (auto& d : m_owner->getDirectDependencies()){
            d->repointStatesFor(m_owner);
        }

        // clean up
        deallocateData(oldData);
    }

    void StateTable::addBorrower(BorrowingNumberSource* borrower){
        assert(
            std::count_if(
                m_slotItems.begin(),
                m_slotItems.end(),
                [&](const SlotItem& si){ return si.borrower == borrower; }
            ) == 0
        );

        auto mainAllocator = getMainAllocator();

        auto allocator = borrower->makeAllocater();
        auto size = allocator->getSize();
        auto align = allocator->getAlignment();
        auto nextOffset = mainAllocator->getSize();

        auto item = m_slotItems.begin();

        // Find the first position whose size is smaller than that of the new state
        while (item != m_slotItems.end()){
            const auto itemSize = item->allocator->getSize();
            if (itemSize < size){
                break;
            }
            item->previousOffset = item->offset;
            nextOffset = item->offset + itemSize;
            ++item;
        }
 
        // insert the new slot item
        nextOffset = nextAlignedOffset(nextOffset, align);
        item = m_slotItems.insert(item, SlotItem{borrower, std::move(allocator), nextOffset, static_cast<size_t>(-1)});
        borrower->m_stateOffset = nextOffset;
        nextOffset += size;

        // shuffle all remaining slot items over
        ++item;
        while (item != m_slotItems.end()){
            item->previousOffset = item->offset;
            nextOffset = nextAlignedOffset(nextOffset, item->allocator->getAlignment());
            item->offset = nextOffset;
            item->borrower->m_stateOffset = nextOffset;
            nextOffset += item->allocator->getSize();
            ++item;
        }

        // infer position of next slot and update size
        nextOffset = nextAlignedOffset(nextOffset, mainAllocator->getAlignment());
        const auto oldSlotSize = m_slotSize;
        m_slotSize = nextOffset;
        assert(oldSlotSize < m_slotSize);

        // allocate new data
        auto oldData = m_data;
        m_data = allocateData(m_slotSize, numSlots());

        // move everything
        for (size_t i = 0, iEnd = numSlots(); i < iEnd; ++i){
            moveSlotAndAddItem(oldData + (i * oldSlotSize), m_data + (i * m_slotSize), borrower);
        }

        // clean up
        deallocateData(oldData);

        // propagate changes
        for (const auto& d : m_owner->getDirectDependencies()){
            d->repointStatesFor(m_owner);
        }
    }

    void StateTable::removeBorrower(BorrowingNumberSource* borrower){
        assert(
            std::count_if(
                m_slotItems.begin(),
                m_slotItems.end(),
                [&](const SlotItem& si){ return si.borrower == borrower; }
            ) == 1
        );

        auto mainAllocator = getMainAllocator();

        auto nextOffset = mainAllocator->getSize();;

        auto item = m_slotItems.begin();

        // Find the slot item for the given borrower
        while (item != m_slotItems.end() && item->borrower != borrower){
            item->previousOffset = item->offset;
            nextOffset = item->offset + item->allocator->getSize();
            ++item;
        }
 
        // record its position
        assert(item != m_slotItems.end());
        assert(item->borrower == borrower);
        const auto itemToRemove = item;
        item->previousOffset = item->offset;

        // NOTE: the slot item is not removed yet so that moveSlotAndRemoveItem
        // still has access to the data it needs for cleanup

        // shuffle all remaining slot items over
        ++item;
        while (item != m_slotItems.end()){
            item->previousOffset = item->offset;
            nextOffset = nextAlignedOffset(nextOffset, item->allocator->getAlignment());
            item->offset = nextOffset;
            item->borrower->m_stateOffset = nextOffset;
            nextOffset += item->allocator->getSize();
            ++item;
        }

        // infer position of next slot and update size
        nextOffset = nextAlignedOffset(nextOffset, mainAllocator->getAlignment());
        const auto oldSlotSize = m_slotSize;
        m_slotSize = nextOffset;
        assert(oldSlotSize > m_slotSize);

        // allocate new data
        auto oldData = m_data;
        m_data = allocateData(m_slotSize, numSlots());

        // move everything
        for (size_t i = 0, iEnd = numSlots(); i < iEnd; ++i){
            moveSlotAndRemoveItem(oldData + (i * oldSlotSize), m_data + (i * m_slotSize), borrower);
        }

        // erase the old slot item
        m_slotItems.erase(itemToRemove);
        borrower->m_stateOffset = static_cast<size_t>(-1);

        // clean up
        deallocateData(oldData);

        // propagate changes
        for (const auto& d : m_owner->getDirectDependencies()){
            d->repointStatesFor(m_owner);
        }
    }

    void StateTable::enableMonostate(bool enable){
        if (enable == m_isMonostate){
            return;
        }

        // ensure allocation things are initialized
        getMainAllocator();

        if (enable){
            // Erase any old data
            if (numSlots() > 0){
                for (auto& d : m_owner->getDirectDependencies()){
                    d->eraseDependentStates(m_owner, 0, numSlots());
                }
                for (size_t i = 0; i < numSlots(); ++i){
                    destroySlot(m_data + (m_slotSize * i));
                }
            }
            deallocateData(m_data);

            // allocate space for one new slot
            m_data = allocateData(m_slotSize, 1);

            // get pointer to dependent state
            const auto& dependents = m_owner->getDirectDependents();
            assert(dependents.size() < 2);
            const SoundState* dependentState = nullptr;
            if (dependents.size() == 1){
                assert(dependents[0]->numSlots() < 2);
                if (dependents[0]->numSlots() == 1){
                    dependentState = dependents[0]->getState(0);
                }
            }

            // construct new slot and point to dependent state
            constructSlot(m_data);
            m_numKeys = 1;
            m_numDependentStates = 1;
            getState(0)->m_dependentState = dependentState;

            // propagate changes
            for (const auto& d : m_owner->getDirectDependencies()){
                d->repointStatesFor(m_owner);
            }

        } else {
            // TODO
            assert(false);
        }
        m_isMonostate = enable;
    }

    bool StateTable::hasMonostate() const noexcept {
        return m_isMonostate;
    }

} // namespace flo
