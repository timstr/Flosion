#pragma once

#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/Objects/FlipFlop.hpp>

namespace flui {

    class FlipFlop : public BorrowingNumberObject {
    public:
        FlipFlop();

    private:
        flo::FlipFlop m_flipFlop;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
