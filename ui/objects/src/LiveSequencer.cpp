#include <Flosion/UI/Objects/LiveSequencer.hpp>

#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <FLosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <GUI/Helpers/ToggleButton.hpp>
#include <GUI/Helpers/NumberTextEntry.hpp>

#include <Flosion/Util/RNG.hpp>
#include <Flosion/Util/Pi.hpp>
#include <GUI/Context.hpp>

namespace flui {

    namespace detail {
        ui::Color randomColor() {
            auto& rng = util::getRandomEngine();
            auto colorDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
            ui::Color c;
            c.setAlpha(1.0f);
            c.setHue(colorDist(rng));
            c.setSaturation(0.5f + 0.5f * colorDist(rng));
            c.setLightness(0.25f + 0.3f * colorDist(rng));
            return c;
        }

        std::string toString(flo::Track::Mode mode) {
            switch (mode) {
            case flo::Track::Mode::LiveOnce:
                return "Live Once";
            case flo::Track::Mode::LiveOngoing:
                return "Live Ongoing";
            case flo::Track::Mode::LiveRestarting:
                return "Live Restarting";
            case flo::Track::Mode::Pause:
                return "Pause";
            case flo::Track::Mode::RecordedInput:
                return "Recorded";
            default:
                return "???";
            };
        }
    }

    LiveSequencer::LiveSequencer()
        : SoundObject(&m_liveSequencer)
        , m_trackContainer(nullptr)
        , m_currentTrack(nullptr)
        , m_bpm(130)
        , m_numBeats(4)
        , m_volumeStep(0.05) {

        updateLiveSequencerLength();

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
            std::make_pair(ui::String("Commands Off"), ui::String("Commands On"))
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

        m_trackContainer = &bod->push_back<ui::VerticalList>();

        bod->push_back<ui::CallbackButton>(
            "Add Track",
            getFont(),
            [&]() {
                m_liveSequencer.addTrack();
            }
        );

        m_onAddTrackConn = m_liveSequencer.onAddTrack.connect([&](flo::Track* t) {
            assert(m_trackContainer);
            m_trackContainer->push_back<TrackUI>(*this, t);
            });

        m_onRemoveTrackConn = m_liveSequencer.onRemoveTrack.connect([&](flo::Track* t) {
            // TODO: assertions
            for (auto it = begin(m_trackUIs); it != end(m_trackUIs); ++it) {
                if ((*it)->track() == t) {
                    (*it)->close();
                    return;
                }
            }
            });

        setBody(std::move(bod));

        addToOutflow(makePeg(&m_liveSequencer));

        addToInflow(makePeg(&m_liveSequencer.currentTime));
    }

    LiveSequencer::~LiveSequencer() {
        m_trackContainer->clear();
        assert(m_trackUIs.size() == 0);
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
                        m_currentTrack->setNextMode(flo::Track::Mode::Pause);
                    }
                }
            ));

            // play live next (ctrl-shift-l, "live")
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::L,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->setNextMode(flo::Track::Mode::LiveOngoing);
                    }
                }
            ));

            // play live once, then repeat (ctrl-shift-t)
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::T,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->setNextMode(flo::Track::Mode::LiveOnce);
                    }
                }
            ));

            // play live, resetting always (ctrl-shift-y)
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::Y,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->setNextMode(flo::Track::Mode::LiveRestarting);
                    }
                }
            ));

            // repeat next (ctrl-shift-r, "repeat")
            m_commands.push_back(win->addKeyboardCommand(
                ui::Key::R,
                { ui::Key::LControl, ui::Key::LShift },
                [&]() {
                    if (m_currentTrack) {
                        m_currentTrack->setNextMode(flo::Track::Mode::RecordedInput);
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

    void LiveSequencer::updateLiveSequencerLength(){
        const auto beatRate = static_cast<double>(m_bpm) / 60.0;
        const auto seconds = static_cast<double>(m_numBeats) / beatRate;
        const auto sf = static_cast<double>(flo::Sample::frequency);
        const auto samples = static_cast<std::size_t>(std::round(seconds * sf));
        m_liveSequencer.setLength(samples);
    }

    Object::FlowDirection LiveSequencer::getNewFlowDirection(FlowDirection fd) const {
        return FlowDirection::Right;
    }

    void LiveSequencer::serialize(Serializer& s) const {
        throw SerializationException{};
        // TODO
    }

    void LiveSequencer::deserialize(Deserializer& d) {
        throw SerializationException{};
        // TODO
    }

    LiveSequencer::TrackUI::TrackUI(LiveSequencer& parent, flo::Track* track)
        : m_parent(parent)
        , m_track(track)
        , m_baseColor(detail::randomColor())
        , m_active(false) {

        m_parent.m_trackUIs.push_back(this);

        setMinSize({450.0f, 80.0f});
        setBackgroundColor(m_baseColor);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);

        // input peg
        adopt(
            ui::FreeContainer::OutsideLeft,
            ui::FreeContainer::Center,
            m_parent.makePeg(&m_track->input)
        );

        // current mode
        auto currentModeLabel = &add<ui::Text>(
            ui::FreeContainer::InsideLeft,
            ui::FreeContainer::InsideTop,
            detail::toString(m_track->currentMode()),
            getFont(),
            0xFFFFFFFF
        );
        m_connections.push_back(
            m_track->onChangeCurrentMode.connect(
                [currentModeLabel](flo::Track::Mode newMode) {
                    currentModeLabel->setText(detail::toString(newMode));
                }
            )
        );

        // next mode buttons (current one lit up)
        auto& btnList = add<ui::HorizontalList>(
            ui::FreeContainer::InsideLeft,
            ui::FreeContainer::InsideBottom
        );
        btnList.setPadding(2.0f);
        btnList.push_back<ui::Text>(
            "Next:",
            getFont(),
            0xFFFFFFC0,
            15,
            ui::TextStyle::Italic
        );
        auto pauseNextBtn = &btnList.push_back<ui::CallbackButton>(
            "Pause",
            getFont(),
            [&]() {
                m_track->setNextMode(flo::Track::Mode::Pause);
            }
        );
        auto repeatNextBtn = &btnList.push_back<ui::CallbackButton>(
            "Recorded",
            getFont(),
            [&]() {
                m_track->setNextMode(flo::Track::Mode::RecordedInput);
            }
        );
        auto playLiveNextBtn = &btnList.push_back<ui::CallbackButton>(
            "Live",
            getFont(),
            [&]() {
                m_track->setNextMode(flo::Track::Mode::LiveOngoing);
            }
        );
        auto playLiveOnceNextBtn = &btnList.push_back<ui::CallbackButton>(
            "Live 1",
            getFont(),
            [&]() {
                m_track->setNextMode(flo::Track::Mode::LiveOnce);
            }
        );
        auto playLiveRepeatingNextBtn = &btnList.push_back<ui::CallbackButton>(
            "Live Rpt",
            getFont(),
            [&]() {
                m_track->setNextMode(flo::Track::Mode::LiveOnce);
            }
        );
        m_connections.push_back(
            m_track->onChangeNextMode.connect(
                [pauseNextBtn, repeatNextBtn, playLiveNextBtn, playLiveOnceNextBtn, playLiveRepeatingNextBtn](flo::Track::Mode newMode) {
                    pauseNextBtn->setNormalColor(0xC0C0C080);
                    repeatNextBtn->setNormalColor(0xC0C0C080);
                    playLiveNextBtn->setNormalColor(0xC0C0C080);
                    playLiveOnceNextBtn->setNormalColor(0xC0C0C080);
                    playLiveRepeatingNextBtn->setNormalColor(0xC0C0C080);
                    if (newMode == flo::Track::Mode::Pause) {
                        pauseNextBtn->setNormalColor(0xFFFFFFC0);
                    } else if (newMode == flo::Track::Mode::RecordedInput) {
                        repeatNextBtn->setNormalColor(0xFFFFFFC0);
                    } else if (newMode == flo::Track::Mode::LiveOngoing) {
                        playLiveNextBtn->setNormalColor(0xFFFFFFC0);
                    } else if (newMode == flo::Track::Mode::LiveOnce) {
                        playLiveOnceNextBtn->setNormalColor(0xFFFFFFC0);
                    } else if (newMode == flo::Track::Mode::LiveRestarting) {
                        playLiveRepeatingNextBtn->setNormalColor(0xFFFFFFC0);
                    } 
                }
            )
        );

        // reset now button
        add<ui::CallbackButton>(
            ui::FreeContainer::Center,
            ui::FreeContainer::InsideTop,
            "Reset",
            getFont(),
            [&]() {
                m_track->resetNow();
            }
        );

        // Volume widget, with minus button on left and plus button on right
        auto& volumeCont = add<ui::HorizontalList>(
            ui::FreeContainer::InsideRight,
            ui::FreeContainer::InsideTop
        );
        volumeCont.setPadding(2.0f);
        volumeCont.push_back<ui::CallbackButton>(
            "-",
            getFont(),
            [&]() {
                m_track->setVolume(m_track->volume() - m_parent.m_volumeStep);
            }
        );

        auto volumeSliderCont = &volumeCont.push_back<ui::Boxed<ui::FreeContainer>>();
        volumeSliderCont->setSize({60.0f, 15.0f}, true);
        volumeSliderCont->setBackgroundColor(0xFF);
        volumeSliderCont->setBorderColor(0xFF);
        volumeSliderCont->setBorderThickness(1.0f);
        auto volumeSlider = &volumeSliderCont->add<ui::BoxElement>(
            ui::FreeContainer::InsideLeft,
            ui::FreeContainer::Center
        );
        const auto currWidth = static_cast<float>(
            m_track->volume() / m_parent.m_liveSequencer.maxVolume()
        ) * 60.0f;
        volumeSlider->setWidth(currWidth, true);
        volumeSlider->setHeight(15.0f, true);
        volumeSlider->setBackgroundColor(0x00FF00FF);

        volumeCont.push_back<ui::CallbackButton>(
            "+",
            getFont(),
            [&]() {
                m_track->setVolume(m_track->volume() + m_parent.m_volumeStep);
            }
        );

        m_connections.push_back(
            m_track->onChangeVolume.connect(
                [this, volumeSlider](double v) {
                    const auto currWidth = static_cast<float>(
                        m_track->volume() / m_parent.m_liveSequencer.maxVolume()
                    ) * 60.0f;
                    volumeSlider->setWidth(currWidth, true);
                    auto c = ui::Color(v > 1.0 ? 0xFF0000FF : 0x00FF00FF);
                    volumeSlider->setBackgroundColor(c);
                }
            )
        );

        // remove button
        add<ui::CallbackButton>(
            ui::FreeContainer::InsideRight,
            ui::FreeContainer::InsideBottom,
            "Delete",
            getFont(),
            [&]() {
                m_parent.m_liveSequencer.removeTrack(m_track);
            }
        ).setNormalColor(0xFF0000C0);
        
        // TODO: Waveform???

        // Highlight when selected
        m_connections.push_back(
            m_parent.m_onSelectTrack.connect(
                [this](flo::Track* t) {
                    if (t == m_track) {
                        m_active = true;
                    } else {
                        m_active = false;
                        setBackgroundColor(m_baseColor);
                    }
                }
            )
        );
    }

    LiveSequencer::TrackUI::~TrackUI(){
        auto& v = m_parent.m_trackUIs;
        assert(count(begin(v), end(v), this) == 1);
        v.erase(remove(begin(v), end(v), this), end(v));

        m_connections.clear();
    }

    void LiveSequencer::TrackUI::render(sf::RenderWindow& rw){
        if (m_active) {
            auto c = m_baseColor;
            auto t = static_cast<float>(ui::Context::get().getProgramTime().asMilliseconds()) / 1000.0f;
            auto x = 0.5f + 0.5f * std::sin(t * 4.0f * pi<float>);
            c.setLightness(c.lightness() + 0.1 * x);
            setBackgroundColor(c);
        }

        ui::Boxed<ui::FreeContainer>::render(rw);

        const auto& ls = m_parent.m_liveSequencer;
        const auto t = static_cast<float>(ls.currentPosition()) / static_cast<float>(ls.length());
        const auto x = t * width();

        auto verts = std::array<sf::Vertex, 2>{
            sf::Vertex(sf::Vector2f{ x, 0.0f }, sf::Color(0xFF0000FF)),
            sf::Vertex(sf::Vector2f{ x, height() }, sf::Color(0xFF0000FF))
        };

        rw.draw(verts.data(), verts.size(), sf::PrimitiveType::Lines);
    }

    bool LiveSequencer::TrackUI::onLeftClick(int){
        m_parent.m_currentTrack = m_track;
        m_parent.m_onSelectTrack.broadcast(m_track);
        return true;
    }

    flo::Track* LiveSequencer::TrackUI::track() noexcept {
        return m_track;
    }

    RegisterFactoryObject(LiveSequencer, "LiveSequencer");
    REGISTER_SERIALIZABLE(LiveSequencer, "LiveSequencer");

} // namespace flui
