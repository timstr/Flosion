#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Resampler.hpp>

namespace flui {

    class Resampler : public Object {
    public:
        Resampler();

    private:
        flo::Resampler m_resampler;
    };

} // namespace flui
