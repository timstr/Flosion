#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Lowpass.hpp>

namespace flui {

    class Lowpass : public SoundObject {
    public:
        Lowpass();

    private:
        flo::WithCurrentTime<flo::Lowpass> m_lowpass;
    };

} // namespace flui
