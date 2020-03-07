#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/LiveSequencer.hpp>

namespace flui {

    class LiveSequencer : public SoundObject {
    public:
        LiveSequencer();
        ~LiveSequencer();

    private:
        flo::LiveSequencer m_liveSequencer;

        void useCommands(bool);

        void previousTrack();

        void nextTrack();

        flo::Signal<flo::Track*> m_onSelectTrack;

        std::vector<ui::KeyboardCommand> m_commands;

        flo::Connection m_onAddTrackConn;
        flo::Connection m_onRemoveTrackConn;

        ui::VerticalList* m_trackContainer;
        flo::Track* m_currentTrack;

        class TrackUI : public ui::Boxed<ui::FreeContainer>, public ui::Control {
        public:
            TrackUI(LiveSequencer& parent, flo::Track* track);
            ~TrackUI();

            void render(sf::RenderWindow&) override;

            bool onLeftClick(int) override;

            flo::Track* track() noexcept;

        private:
            LiveSequencer& m_parent;
            flo::Track* const m_track;
            const ui::Color m_baseColor;
            bool m_active;

            std::vector<flo::Connection> m_connections;
        };

        std::vector<TrackUI*> m_trackUIs;

        int m_bpm;
        int m_numBeats;

        const double m_volumeStep;

        void updateLiveSequencerLength();

        FlowDirection getNewFlowDirection(FlowDirection) const override;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
