#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Resampler.hpp>

namespace flui {

    class Resampler : public SoundObject {
    public:
        Resampler();

    private:
        flo::Resampler m_resampler;
    };

} // namespace flui
