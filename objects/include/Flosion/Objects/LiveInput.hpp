#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <SFML/Audio/SoundRecorder.hpp>

#include <array>

namespace flo {

    class LiveInputState : public SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

        static constexpr std::size_t buffer_size = 4096;

        // TODO: synchronization
        std::array<Sample, buffer_size> buffer;
        std::size_t producer_pos;
        std::size_t consumer_pos;
    };

    class LiveInput : public WithCurrentTime<Realtime<UncontrolledSoundSource<LiveInputState>>> {
    public:
        LiveInput();
        ~LiveInput();

        void start();

        void stop();

        // bool isPlaying() const;

        bool isAvailable() const;

        std::vector<std::string> getAvailableDevices() const;

        std::string getDefaultDevice() const;

        void selectDevice(const std::string&);

        const std::string& getDevice() const;

    private:

        void renderNextChunk(SoundChunk&, LiveInputState*) override;

        class Recorder : public sf::SoundRecorder {
        public:
            Recorder(LiveInput& parent);
            ~Recorder();

        private:
            bool onProcessSamples(const sf::Int16* samples, std::size_t sampleCount) override;

            LiveInput& m_parent;
        };

        Recorder m_recorder;

    };

} // namespace flo
