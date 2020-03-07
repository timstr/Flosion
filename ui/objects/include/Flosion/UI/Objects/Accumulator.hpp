#pragma once

#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/Objects/Accumulator.hpp>

namespace flui {

    class Accumulator : public BorrowingNumberObject {
    public:
        Accumulator();

    private:
        flo::Accumulator m_accumulator;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
