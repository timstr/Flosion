#include <Flosion/UI/Objects/AudioClip.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

#include <GUI/Helpers/CallbackButton.hpp>
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

        s << static_cast<std::uint64_t>(b.getSampleCount());
        s << static_cast<std::uint64_t>(b.getChannelCount());
        s << static_cast<std::uint64_t>(b.getSampleRate());
        s.writeSpan(b.getSamples(), n);
        s << m_label->text().toAnsiString();
    }

    void AudioClip::deserialize(Deserializer& s){
        deserializePegs(s);

        auto sc = std::uint64_t{};
        auto cc = std::uint64_t{};
        auto sr = std::uint64_t{};

        std::vector<std::int16_t> audioData;

        std::string txt;

        s >> sc >> cc >> sr >> audioData >> txt;

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
