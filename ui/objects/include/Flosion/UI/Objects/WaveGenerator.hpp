#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/WaveGenerator.hpp>

namespace flui {

    class WaveGenerator : public Object {
    public:
        WaveGenerator();

    private:
        flo::WaveGenerator m_waveGen;
    };

} // namespace flui
