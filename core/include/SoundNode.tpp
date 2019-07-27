#include <cassert>
#include "SoundNode.hpp"

namespace flo {

    // Allocatable

    template<typename SoundNodeType, typename SoundStateType>
    inline std::unique_ptr<StateAllocator> Allocatable<SoundNodeType, SoundStateType>::makeAllocator() const     {
        return std::make_unique<ConcreteStateAllocator<SoundStateType>>();
    }


    // Singular

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


    // Divergent

    template<typename SoundNodeType, typename SoundStateType, typename KeyType>
    inline bool Divergent<SoundNodeType, SoundStateType, KeyType>::isDivergent() const noexcept {
        return true;
    }
    

    // Controllable

    template<typename SoundNodeType>
    inline bool Controllable<SoundNodeType>::isUncontrolled() const noexcept {
        return false;
    }


    // Uncontrolled

    template<typename SoundNodeType>
    const SoundState* Uncontrolled<SoundNodeType>::getMonoState() const noexcept {
        assert(numSlots() == 1);
        return getState(0);
    }

    template<typename SoundNodeType>
    inline bool Uncontrolled<SoundNodeType>::isUncontrolled() const noexcept {
        return true;
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

