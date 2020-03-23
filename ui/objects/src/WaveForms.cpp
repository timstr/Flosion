#include <Flosion/UI/Objects/WaveForms.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>

#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

namespace flui {

    SineWave::SineWave(){
        addToInflow(makePeg(&m_sineWave.input));
        addToOutflow(makePeg(&m_sineWave));

        setBody(makeSimpleBody("Sine Wave", 0x402060FF));
    }

    void SineWave::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void SineWave::deserialize(Deserializer& d) {
        deserializePegs(d);
    }

    RegisterFactoryObject(SineWave, "Sine Wave", "Unit Sine");
    REGISTER_SERIALIZABLE(SineWave, "SineWave");

    SawWave::SawWave(){
        addToInflow(makePeg(&m_sawWave.input));
        addToOutflow(makePeg(&m_sawWave));

        setBody(makeSimpleBody("Saw Wave", 0x402060FF));
    }

    void SawWave::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void SawWave::deserialize(Deserializer& d) {
        deserializePegs(d);
    }

    RegisterFactoryObject(SawWave, "Saw Wave");
    REGISTER_SERIALIZABLE(SawWave, "SawWave");

    SquareWave::SquareWave(){
        addToInflow(makePeg(&m_squareWave.input));
        addToOutflow(makePeg(&m_squareWave));

        setBody(makeSimpleBody("Square Wave", 0x402060FF));
    }

    void SquareWave::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void SquareWave::deserialize(Deserializer& d) {
        deserializePegs(d);
    }

    RegisterFactoryObject(SquareWave, "Square Wave");
    REGISTER_SERIALIZABLE(SquareWave, "SquareWave");

    TriangleWave::TriangleWave(){
        addToInflow(makePeg(&m_triangleWave.input));
        addToOutflow(makePeg(&m_triangleWave));

        setBody(makeSimpleBody("Triangle Wave", 0x402060FF));
    }

    void TriangleWave::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void TriangleWave::deserialize(Deserializer& d) {
        deserializePegs(d);
    }

    RegisterFactoryObject(TriangleWave, "Triangle Wave");
    REGISTER_SERIALIZABLE(TriangleWave, "TriangleWave");

    PulseWave::PulseWave() {
        addToInflow(makePeg(&m_pulseWave.input, "Input"));
        addToInflow(makePeg(&m_pulseWave.width, "Width"));
        addToOutflow(makePeg(&m_pulseWave, "Output"));

        setBody(makeSimpleBody("Pulse Wave", 0x402060FF));
    }

    void PulseWave::serialize(Serializer& s) const {
        serializePegs(s);
    }

    void PulseWave::deserialize(Deserializer& d) {
        deserializePegs(d);
    }

    RegisterFactoryObject(PulseWave, "Pulse Wave");
    REGISTER_SERIALIZABLE(PulseWave, "PulseWave");

} // namespace flui
