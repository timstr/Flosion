#include <Flosion/UI/Core/NumberObject.hpp>
#include <Flosion/Objects/WaveForms.hpp>

namespace flui {

    class SineWave : public NumberObject {
    public:
        SineWave();

    private:
        flo::SineWave m_sineWave;
    };

    class SawWave : public NumberObject {
    public:
        SawWave();

    private:
        flo::SawWave m_sawWave;
    };

    class TriangleWave : public NumberObject {
    public:
        TriangleWave();

    private:
        flo::TriangleWave m_triangleWave;
    };

    class SquareWave : public NumberObject {
    public:
        SquareWave();

    private:
        flo::SquareWave m_squareWave;
    };

} // namespace flui
