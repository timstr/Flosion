#pragma once

#include <State.hpp>

namespace flo {

    class SoundState;

    // TODO: distinguish between SoundState and default-constructible state

    class StateAllocator {
    public:
        virtual ~StateAllocator() noexcept = default;

        virtual void construct(void* dst, SoundNode* owner, const SoundState* dependentState) = 0;
        virtual void moveConstruct(void* dst, void* src) noexcept = 0;
        virtual void destroy(void* dst) noexcept = 0;

        virtual size_t getSize() const noexcept = 0;
        virtual size_t getAlignment() const noexcept = 0;
    };

    

    template<typename StateType>
    class ConcreteStateAllocator : public StateAllocator {
    private:
        void construct(void* dst, SoundNode* owner, const SoundState* dependentState) override final {
            new (dst) StateType(owner, dependentState);
        }
        void moveConstruct(void* dst, void* src) noexcept override final {
            new (dst) StateType(std::move(*static_cast<StateType*>(src)));
        }
        void destroy(void* dst) noexcept override final {
            (*static_cast<StateType*>(dst)).~StateType();
        }

        size_t getSize() const noexcept override final {
            return sizeof(StateType);
        }
        size_t getAlignment() const noexcept override final {
            return alignof(StateType);
        }
    };

} // namespace flo
