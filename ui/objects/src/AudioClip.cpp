#include <Flosion/UI/Objects/AudioClip.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

#include <GUI/Helpers/CallbackButton.hpp>
#include <GUI/Helpers/ToggleButton.hpp>
#include <Flosion/Util/FileBrowser.hpp>

namespace flui {

    AudioClip::AudioClip()
        : SoundObject(&m_audioClip) {

        // TODO
        addToOutflow(makePeg(&m_audioClip));


        auto bod = std::make_unique<ui::Boxed<ui::VerticalList>>();
        m_label = &bod->push_back<ui::Text>("AudioClip", getFont());
        bod->push_back<ui::CallbackButton>(
            "Load",
            getFont(),
            [&](){
                auto fd = util::FileDialog{};
                fd.addFilter(L"Sound Files", {L"wav", L"ogg", L"flac"});
                if (auto p = fd.openFile(); !p.empty()) {
                    loadFromFile(p.string());
                }
            }
        );

        bod->push_back<ui::ToggleButton>(
            m_audioClip.looping(),
            getFont(),
            [&](bool l){
                m_audioClip.setLooping(l);
            },
            std::make_pair(ui::String("One-shot"), ui::String("Loop"))
        );

        bod->setPadding(5.0f);
        bod->setBackgroundColor(0x2f8700ff);
        setBody(std::move(bod));
    }

    void AudioClip::loadFromFile(const std::string& path){
        m_audioClip.loadFromFile(path);
        if (auto i = path.find_last_of("/\\"); i != std::string::npos){
            m_label->setText(path.substr(i + 1));
        } else {
            m_label->setText("");
        }
    }

    void AudioClip::serialize(Serializer& s) const {
        serializePegs(s);

        const auto& b = m_audioClip.getSoundBuffer();
        const auto n = b.getSampleCount() * b.getChannelCount();

        s.u64(b.getSampleCount()).u64(b.getChannelCount()).u64(b.getSampleRate());

        s.i16_span(b.getSamples(), n);

        s.str(m_label->text().toAnsiString());
    }

    void AudioClip::deserialize(Deserializer& d){
        deserializePegs(d);

        auto sc = d.u64();
        auto cc = d.u64();
        auto sr = d.u64();
        auto audioData = d.i16_vec();
        auto txt = d.str();

        assert(audioData.size() == sc * cc);

        m_audioClip.getSoundBuffer().loadFromSamples(
            audioData.data(),
            sc,
            static_cast<unsigned int>(cc),
            static_cast<unsigned int>(sr)
        );

        m_label->setText(txt);
    }

    RegisterFactoryObject(AudioClip, "AudioClip");
    REGISTER_SERIALIZABLE(AudioClip, "AudioClip");

} // namespace flui
