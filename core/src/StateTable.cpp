#include <StateTable.hpp>
#include <StateAllocator.hpp>

namespace flo {
    StateTable::StateTable(SoundNetwork* network, SoundNode::Type type, std::unique_ptr<StateAllocator> mainAllocator)
        : SoundNode(network, type)
        , m_mainAllocator(std::move(mainAllocator))
        , m_data(allocateData(m_mainAllocator->getSize(), 1, m_mainAllocator->getAlignment()))
        , m_slotSize(m_mainAllocator->getSize())
        , m_numSlots(1)
        , m_capacity(1) {

    }

    StateTable::~StateTable(){
        for (size_t i = 0; i < m_numSlots; ++i){
            destroySlot(m_data + (i * m_slotSize));
        }

        deallocateData(m_data);
    }

    unsigned char* StateTable::allocateData(size_t slotSize, size_t numSlots, size_t alignment){
        const auto align = std::align_val_t{m_mainAllocator->getAlignment()};
        void* ptr = operator new(m_mainAllocator->getSize(), align);
        return static_cast<unsigned char*>(ptr);
    }

    void StateTable::deallocateData(unsigned char* ptr){
        operator delete(static_cast<void*>(ptr));
    }

} // namespace flo
