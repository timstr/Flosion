#pragma once

#include <State.hpp>

namespace flo {

    class StateAllocator {
    public:
        virtual ~StateAllocator() = default;

        virtual void construct(void* dst) = 0;
        virtual void moveConstruct(void* dst, void* src) noexcept = 0;
        virtual void destroy(void* dst) noexcept = 0;

        virtual size_t getSize() const noexcept = 0;
        virtual size_t getAlignment() const noexcept = 0;
    };

    

    template<typename StateType>
    class ConcreteStateAllocator {
    private:
        void construct(void* dst) override {
            new (dst) StateType();
        }
        void moveConstruct(void* dst, void* src) noexcept override {
            new (dst) StateType(std::move(*static_cast<StateType*>(src)));
        }
        void destroy(void* dst) noexcept override {
            (*static_cast<StateType*>(dst)).~StateType();
        }

        size_t getSize() const noexcept override {
            return sizeof(StateType);
        }
        size_t getAlignment() const noexcept override {
            return alignof(StateType);
        }
    };

} // namespace flo
