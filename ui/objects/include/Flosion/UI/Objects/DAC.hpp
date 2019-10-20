#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/DAC.hpp>

namespace flui {

    class DAC : public Object {
    public:
        DAC();

    private:
        flo::DAC m_dac;
    };

} // namespace flui
