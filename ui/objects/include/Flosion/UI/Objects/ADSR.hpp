#pragma once

#include <Flosion/UI/Core/NumberObject.hpp>
#include <Flosion/Objects/ADSR.hpp>

namespace flui {

    class ADSR : public NumberObject {
    public:
        ADSR();

    private:
        flo::ADSR m_adsr;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
