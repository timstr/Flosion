#include <Flosion/Objects/LiveInput.hpp>

namespace flo {

    void LiveInputState::reset() noexcept {
        producer_pos = 0;
        consumer_pos = 0;
    }

    LiveInput::LiveInput()
        : m_recorder(*this) {

        // TODO: hack
        enableMonostate();
    }

    LiveInput::~LiveInput(){
        m_recorder.stop();
    }

    void LiveInput::start(){
        m_recorder.start(Sample::frequency);
    }

    void LiveInput::stop(){
        m_recorder.stop();
        auto& b = getMonoState()->buffer;
        for (auto& s : b) {
            s.silence();
        }
    }

    bool LiveInput::isAvailable() const {
        return sf::SoundRecorder::isAvailable();
    }

    std::vector<std::string> LiveInput::getAvailableDevices() const {
        return sf::SoundRecorder::getAvailableDevices();
    }

    std::string LiveInput::getDefaultDevice() const {
        return sf::SoundRecorder::getDefaultDevice();
    }

    void LiveInput::selectDevice(const std::string& s){
        m_recorder.setDevice(s);
    }

    const std::string& LiveInput::getDevice() const {
        return m_recorder.getDevice();
    }

    void LiveInput::renderNextChunk(SoundChunk& chunk, LiveInputState* state) {
        // read data from monostate's buffer, don't surpass producer
        // TODO: this is unsafe! Some kind of synchronization is needed
        std::size_t count = 0;
        while (count < SoundChunk::size && (state->consumer_pos + 1) != state->producer_pos) {
            chunk[count] = state->buffer[state->consumer_pos];
            ++count;
            state->consumer_pos = (state->consumer_pos + 1) % state->buffer.size();
        }
    }

    LiveInput::Recorder::Recorder(LiveInput& parent)
        : m_parent(parent) {

        setDevice(getDefaultDevice());
        setChannelCount(2);
        setProcessingInterval(sf::milliseconds(20)); // TODO: tune this
    }

    LiveInput::Recorder::~Recorder() {
        stop();
    }

    bool LiveInput::Recorder::onProcessSamples(const sf::Int16* samples, std::size_t sampleCount){
        // fill parent's monostates' buffer, don't lap consumer
        // TODO: this is unsafe! Some kind of synchronization is needed
        auto s = m_parent.getMonoState();
        constexpr auto k = 1.0f / static_cast<float>(std::numeric_limits<int16_t>::max());
        std::size_t count = 0;
        while (s->producer_pos != s->consumer_pos && count < sampleCount) {
            auto l = static_cast<float>(samples[count + 0]) * k;
            auto r = static_cast<float>(samples[count + 1]) * k;
            s->buffer[s->producer_pos] = Sample{l, r};
            count += 2;
            s->producer_pos = (s->producer_pos + 1) % s->buffer.size();
        }
        return true;
    }

} // namespace flo
