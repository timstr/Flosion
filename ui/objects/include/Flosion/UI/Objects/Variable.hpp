#pragma once

#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/Objects/Variable.hpp>

namespace flui {

    class Variable : public BorrowingNumberObject {
    public:
        Variable();

    private:
        flo::Variable m_variable;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
