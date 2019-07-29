#include <cassert>
#include "SoundNode.hpp"

// TODO: make sure to DELETE the "#include <SoundNode.hpp>" that Visual Studio inserts here >:(

namespace flo {

    // Singular

    template<typename SoundNodeType, typename SoundStateType>
    Singular<SoundNodeType, SoundStateType>::Singular(){
        StateTable::insertKeys(0, 1);
    }

    template<typename SoundNodeType, typename SoundStateType>
    SoundStateType* Singular<SoundNodeType, SoundStateType>::getState(const SoundNode* dependent, const SoundState* dependentState) noexcept {
        return reinterpret_cast<SoundStateType*>(StateTable::getState(dependent, dependentState, 0));
    }

    template<typename SoundNodeType, typename SoundStateType>
    const SoundStateType* Singular<SoundNodeType, SoundStateType>::getState(const SoundNode* dependent, const SoundState* dependentState) const noexcept {
        return reinterpret_cast<const SoundStateType*>(StateTable::getState(dependent, dependentState, 0));
    }

    template<typename SoundNodeType, typename SoundStateType>
    SoundStateType* Singular<SoundNodeType, SoundStateType>::getState(size_t index) noexcept {
        return reinterpret_cast<SoundStateType*>(StateTable::getState(index));
    }

    template<typename SoundNodeType, typename SoundStateType>
    const SoundStateType* Singular<SoundNodeType, SoundStateType>::getState(size_t index) const noexcept {
        return reinterpret_cast<const SoundStateType*>(StateTable::getState(index));
    }

    template<typename SoundNodeType, typename SoundStateType>
    inline bool Singular<SoundNodeType, SoundStateType>::isDivergent() const noexcept {
        return false;
    }

    template<typename SoundNodeType, typename SoundStateType>
    bool Singular<SoundNodeType, SoundStateType>::isUncontrolled() const noexcept {
        return false;
    }

    template<typename SoundNodeType, typename SoundStateType>
    std::unique_ptr<StateAllocator> Singular<SoundNodeType, SoundStateType>::makeAllocator() const {
        return std::make_unique<ConcreteStateAllocator<SoundStateType>>();
    }


    // Divergent

    template<typename SoundNodeType, typename SoundStateType, typename KeyType>
    inline bool Divergent<SoundNodeType, SoundStateType, KeyType>::isDivergent() const noexcept {
        return true;
    }

    template<typename SoundNodeType, typename SoundStateType, typename KeyType>
    bool Divergent<SoundNodeType, SoundStateType, KeyType>::isUncontrolled() const noexcept {
        return false;
    }

    template<typename SoundNodeType, typename SoundStateType, typename KeyType>
    std::unique_ptr<StateAllocator> Divergent<SoundNodeType, SoundStateType, KeyType>::makeAllocator() const {
        return std::make_unique<ConcreteStateAllocator<SoundStateType>>();
    }
    
    // Uncontrolled

    template<typename SoundNodeType, typename SoundStateType>
    inline Uncontrolled<SoundNodeType, SoundStateType>::Uncontrolled(){
        StateTable::enableMonostate();
    }

    template<typename SoundNodeType, typename SoundStateType>
    const SoundStateType* Uncontrolled<SoundNodeType, SoundStateType>::getMonoState() const noexcept {
        assert(StateTable::numSlots() == 1);
        assert(StateTable::hasMonostate());
        return reinterpret_cast<const SoundStateType*>(StateTable::getState(0));
    }

    template<typename SoundNodeType, typename SoundStateType>
    bool Uncontrolled<SoundNodeType, SoundStateType>::isDivergent() const noexcept {
        return false;
    }

    template<typename SoundNodeType, typename SoundStateType>
    bool Uncontrolled<SoundNodeType, SoundStateType>::isUncontrolled() const noexcept {
        return true;
    }

    template<typename SoundNodeType, typename SoundStateType>
    std::unique_ptr<StateAllocator> Uncontrolled<SoundNodeType, SoundStateType>::makeAllocator() const {
        return std::make_unique<ConcreteStateAllocator<SoundStateType>>();
    }

    // Realtime

    template<typename SoundNodeType>
    inline bool Realtime<SoundNodeType>::isOutOfSync() const noexcept {
        return false;
    }

    template<typename SoundNodeType>
    inline double Realtime<SoundNodeType>::getTimeSpeed(const SoundState*) const noexcept {
        return 1.0;
    }


    // OutOfSync

    template<typename SoundNodeType>
    inline bool OutOfSync<SoundNodeType>::isOutOfSync() const noexcept {
        return true;
    }

}

