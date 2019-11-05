#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Lowpass.hpp>

namespace flui {

    class Lowpass : public Object {
    public:
        Lowpass();

    private:
        flo::Lowpass m_lowpass;
    };

} // namespace flui
