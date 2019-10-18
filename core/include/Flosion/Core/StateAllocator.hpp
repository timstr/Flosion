#pragma once

#include <Flosion/Core/State.hpp>

#include <cstddef>
#include <type_traits>
#include <utility>

namespace flo {

    class SoundNode;
    class SoundState;

    class StateAllocator {
    public:
        virtual ~StateAllocator() noexcept = default;

        virtual void construct(void* dst, SoundNode* owner, const SoundState* dependentState) = 0;
        virtual void moveConstruct(void* dst, void* src) noexcept = 0;
        virtual void destroy(void* dst) noexcept = 0;

        virtual std::size_t getSize() const noexcept = 0;
        virtual std::size_t getAlignment() const noexcept = 0;
    };

    

    template<typename StateType>
    class ConcreteStateAllocator : public StateAllocator {
    private:
        void construct(void* dst, SoundNode* owner, const SoundState* dependentState) override final {
            if constexpr (std::is_base_of_v<SoundState, StateType>){
                auto s = new (dst) StateType(owner, dependentState);
                s->reset();
            } else if constexpr (std::is_base_of_v<State, StateType>){
                auto s = new (dst) StateType();
                s->reset();
            } else {
                static_assert(!std::is_same_v<StateType, StateType>, "The template parameter passed to ConcreteStateAllocator must derive from State or SoundState.");
            }
        }
        void moveConstruct(void* dst, void* src) noexcept override final {
            new (dst) StateType(std::move(*static_cast<StateType*>(src)));
        }
        void destroy(void* dst) noexcept override final {
            (*static_cast<StateType*>(dst)).~StateType();
        }

        std::size_t getSize() const noexcept override final {
            return sizeof(StateType);
        }
        std::size_t getAlignment() const noexcept override final {
            return alignof(StateType);
        }
    };

} // namespace flo
