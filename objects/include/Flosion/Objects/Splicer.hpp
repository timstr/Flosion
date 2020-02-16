#pragma once

#include <Flosion/Core/SoundSourceTemplate.hpp>
#include <Flosion/Core/SingleSoundInput.hpp>

namespace flo {

    // TODO: restrict inner object/string of objects to be realtime and singular
    // TODO: allow side chain to be reused by a parallel stream (i.e. do not force
    // its sound stream to be discarded)
    // This will require careful planning and synchronization between two streams
    // or branches of the same stream


    class SplicerState : public flo::SoundState {
    public:
        using SoundState::SoundState;

        void reset() noexcept override;

        enum class RouteType : std::uint8_t {
            Main,
            Side
        };

        RouteType currentRoute;
    };

    class Splicer : public flo::Realtime<flo::ControlledSoundSource<SplicerState>> {
    public:
        Splicer();

        SingleSoundInput* getMainInput() noexcept;
        SingleSoundInput* getSideInput() noexcept;

        /**
         * To be safe to install, the SoundSource must:
         * - be singular
         * - be realtime
         * - have exactly one SingleSoundInput
         */
        bool safeToInstall(const SoundSource*) const noexcept;

        void install(SoundSource*);

        const SoundSource* getInstalledSoundSource() const noexcept;

    private:

        class RoutingNode : public flo::Realtime<flo::ControlledSoundSource<EmptySoundState>> {
        public:
            RoutingNode(Splicer&);

            void renderNextChunk(SoundChunk&, EmptySoundState*) override;

        private:
            Splicer& m_parentSplicer;
        };

        RoutingNode m_routingNode;

        SingleSoundInput m_innerInput;

        SingleSoundInput m_mainInput;
        SingleSoundInput m_sideInput;

        SoundSource* m_installedSoundSource;
        SoundInput* m_installedInput;

        void renderNextChunk(SoundChunk&, SplicerState*) override;
    };

} // namespace flo
