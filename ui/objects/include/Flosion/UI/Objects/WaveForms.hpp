#include <Flosion/UI/Core/NumberObject.hpp>
#include <Flosion/Objects/WaveForms.hpp>

namespace flui {

    class SineWave : public NumberObject {
    public:
        SineWave();

    private:
        flo::SineWave m_sineWave;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

    class SawWave : public NumberObject {
    public:
        SawWave();

    private:
        flo::SawWave m_sawWave;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

    class TriangleWave : public NumberObject {
    public:
        TriangleWave();

    private:
        flo::TriangleWave m_triangleWave;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

    class SquareWave : public NumberObject {
    public:
        SquareWave();

    private:
        flo::SquareWave m_squareWave;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

    class PulseWave : public NumberObject {
    public:
        PulseWave();

    private:
        flo::PulseWave m_pulseWave;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
