#include <cassert>

namespace flo {

    template<typename SoundStateType>
    SoundStateType* StateTable::getState(const SoundNode* dependent, const SoundState* dependentState, size_t keyIndex) noexcept {
        return const_cast<SoundStateType*>(
            const_cast<const StateTable*>(this)->getState<SoundStateType>(dependent, dependentState, keyIndex)
        );
    }

    template<typename SoundStateType>
    const SoundStateType* StateTable::getState(const SoundNode* dependent, const SoundState* dependentState, size_t keyIndex) const noexcept {
        auto s = this->getState(dependent, dependentState, keyIndex);
        assert(s);
        auto sd = static_cast<const SoundStateType*>(s);
        assert(sd);
        assert(dynamic_cast<const SoundStateType*>(s) == sd);
        return sd;
    }
    
    template<typename SoundStateType>
    SoundStateType* StateTable::getState(size_t index) noexcept {
        return const_cast<SoundStateType*>(
            const_cast<const StateTable*>(this)->getState<SoundStateType>(index)
        );
    }
    
    template<typename SoundStateType>
    const SoundStateType* StateTable::getState(size_t index) const noexcept {
        auto s = this->getState(index);
        assert(s);
        auto sd = static_cast<const SoundStateType*>(s);
        assert(sd);
        assert(dynamic_cast<const SoundStateType*>(s) == sd);
        return sd;
    }

    template<typename StateType>
    StateType* StateTable::getBorrowedState(const SoundState* mainState, const BorrowingNumberSource* borrower) const noexcept {
        auto s = this->getBorrowedState(mainState, borrower);
        assert(s);
        auto sd = static_cast<StateType*>(s);
        assert(sd);
        assert(dynamic_cast<StateType*>(s) == sd);
        return sd;
    }

    
    template<typename SoundStateType>
    SoundStateType* StateTable::getDerivedState(const SoundState* s) noexcept {
        return const_cast<SoundStateType*>(
            const_cast<const StateTable*>(this)->getDerivedState<SoundStateType>(s)
        );
    }

    template<typename SoundStateType>
    const SoundStateType* StateTable::getDerivedState(const SoundState* s) const noexcept {
        assert(s);
        assert(hasState(s));
        auto sd = static_cast<const SoundStateType*>(s);
        assert(sd);
        assert(dynamic_cast<const SoundStateType*>(s) == sd);
        return sd;
    }

} // namespace flo
