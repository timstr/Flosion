#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/LiveSequencer.hpp>

namespace flui {

    class LiveSequencer : public SoundObject {
    public:
        LiveSequencer();

    private:
        flo::LiveSequencer m_liveSequencer;

        void useCommands(bool);

        void previousTrack();

        void nextTrack();

        flo::Signal<flo::Track*> m_onSelectTrack;

        std::vector<ui::KeyboardCommand> m_commands;

        flo::Connection m_onAddTrackConn;

        ui::VerticalList* m_tracklist;
        flo::Track* m_currentTrack;

        int m_bpm;
        int m_numBeats;

        const double m_volumeStep;

        std::unique_ptr<ui::Element> makeTrack(flo::Track*);

        void updateLiveSequencerLength();

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
