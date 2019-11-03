#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/WaveForms.hpp>

namespace flui {

    class SineWave : public Object {
    public:
        SineWave();

    private:
        flo::SineWave m_sineWave;
    };

    class SawWave : public Object {
    public:
        SawWave();

    private:
        flo::SawWave m_sawWave;
    };

    class TriangleWave : public Object {
    public:
        TriangleWave();

    private:
        flo::TriangleWave m_triangleWave;
    };

    class SquareWave : public Object {
    public:
        SquareWave();

    private:
        flo::SquareWave m_squareWave;
    };

} // namespace flui
