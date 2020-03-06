#include <Flosion/UI/Objects/LiveSequencer.hpp>

#include <Flosion/UI/Core/SoundWire.hpp>
#include <FLosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/NumberTextEntry.hpp>

namespace flui {

    LiveSequencer::LiveSequencer()
        : SoundObject(&m_liveSequencer)
        , m_tracklist(nullptr)
        , m_currentTrack(nullptr)
        , m_bpm(130)
        , m_numBeats(4)
        , m_volumeStep(0.05) {
        
        auto bod = std::make_unique<ui::Boxed<ui::VerticalList>>();
        bod->setBackgroundColor(0xffae00FF);
        bod->setBorderThickness(1.0f);
        bod->setBorderColor(0xFFFFFFFF);
        bod->setPadding(5.0f);
        
        auto& head = bod->push_back<ui::Boxed<ui::HorizontalList>>();
        head.setBackgroundColor(0xd46c04FF);
        head.setPadding(5.0f);

        head.push_back<ui::Text>("Live Sequencer", getFont(), 0xFF);

        auto& config = bod->push_back<ui::Boxed<ui::HorizontalList>>();
        config.setPadding(5.0f);

        config.push_back<ui::ToggleButton>(
            false,
            getFont(),
            [&](bool v) {
                useCommands(v);
            },
            std::make_pair(ui::String("Commands On"), ui::String("Commands Off"))
        );

        config.push_back<ui::Text>("BPM:", getFont(), 0x80, 15, ui::TextStyle::Italic);
        config.push_back<ui::NumberTextEntry<int>>(
            m_bpm,
            60,
            240,
            getFont(),
            [&](int bpm) {
                m_bpm = bpm;
                updateLiveSequencerLength();
            }
        );

        config.push_back<ui::Text>("Beats:", getFont(), 0x80, 15, ui::TextStyle::Italic);
        config.push_back<ui::NumberTextEntry<int>>(
            m_numBeats,
            1,
            16,
            getFont(),
            [&](int beats) {
                m_numBeats = beats;
                updateLiveSequencerLength();
            }
        );

        m_tracklist = &bod->push_back<ui::VerticalList>();

        bod->push_back<ui::CallbackButton>(
            "Add Track",
            getFont(),
            [&]() {
                m_liveSequencer.addTrack();
            }
        );

        m_onAddTrackConn = m_liveSequencer.onAddTrack.connect([&](flo::Track* t) {
            m_tracklist->push_back(makeTrack(t));
        });

        setBody(std::move(bod));

        addToOutflow(makePeg(&m_liveSequencer));

        // TODO: prevent changing flow direction
    }

    void LiveSequencer::useCommands(bool use){
        m_commands.clear();
        if (use) {
            auto win = getParentWindow();
            assert(win);

            // select previous track (ctrl-shift-up)
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::Up,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    previousTrack();
                }
            ));

            // select next track (ctrl-shift-down)
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::Down,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    nextTrack();
                }
            ));

            // pause next (ctrl-shift-p, "pause")
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::P,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->pauseNext();
                    }
                }
            ));

            // play live next (ctrl-shift-l, "live")
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::L,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->playLiveInputNext();
                    }
                }
            ));

            // repeat next (ctrl-shift-r, "repeat")
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::R,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->playRecordedInputNext();
                    }
                }
            ));

            // volume down (ctrl-shift-left)
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::Left,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->setVolume(m_currentTrack->volume() - m_volumeStep);
                    }
                }
            ));

            // volume up (ctrl-shift-right)
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::Right,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->setVolume(m_currentTrack->volume() + m_volumeStep);
                    }
                }
            ));

            // reset/restart input now (ctrl-shift-enter)
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::Enter,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->resetNow();
                    }
                }
            ));
        }
    }

    void LiveSequencer::previousTrack(){
        const auto allTracks = m_liveSequencer.getTracks();
        if (m_currentTrack) {
            const auto it = find(begin(allTracks), end(allTracks), m_currentTrack);
            assert(it != end(allTracks));
            const auto idx = it - begin(allTracks);
            if (idx == 0) {
                m_currentTrack = allTracks.back();
            } else {
                m_currentTrack = allTracks[idx - 1];
            }
        } else if (allTracks.size() > 0) {
            m_currentTrack = allTracks.back();
        } else {
            assert(m_currentTrack == nullptr);
        }
        m_onSelectTrack.broadcast(m_currentTrack);
    }

    void LiveSequencer::nextTrack(){
        const auto allTracks = m_liveSequencer.getTracks();
        if (m_currentTrack) {
            const auto it = find(begin(allTracks), end(allTracks), m_currentTrack);
            assert(it != end(allTracks));
            const auto idx = it - begin(allTracks);
            m_currentTrack = allTracks[(idx + 1) % allTracks.size()];
        } else if (allTracks.size() > 0) {
            m_currentTrack = allTracks.front();
        } else {
            assert(m_currentTrack == nullptr);
        }
        m_onSelectTrack.broadcast(m_currentTrack);
    }

    std::unique_ptr<ui::Element> LiveSequencer::makeTrack(flo::Track* t){
        // TODO:
        auto b = std::make_unique<ui::BoxElement>();
        b->setSize({ 100.0f, 100.0f }, true);
        b->setBackgroundColor(0xFF0000FF);
        b->setBorderColor(0xFF);
        b->setBorderThickness(1.0f);
        return b;
    }

    void LiveSequencer::updateLiveSequencerLength(){
        const auto seconds = static_cast<double>(m_bpm) * static_cast<double>(m_numBeats);
        const auto sf = static_cast<double>(flo::Sample::frequency);
        const auto samples = static_cast<std::size_t>(std::round(seconds * sf));
        m_liveSequencer.setLength(samples);
    }

    void LiveSequencer::serialize(Serializer& s) const {
        throw SerializationException{};
        // TODO
    }

    void LiveSequencer::deserialize(Deserializer& d) {
        throw SerializationException{};
        // TODO
    }

    RegisterFactoryObject(LiveSequencer, "LiveSequencer");
    REGISTER_SERIALIZABLE(LiveSequencer, "LiveSequencer");

} // namespace flui
