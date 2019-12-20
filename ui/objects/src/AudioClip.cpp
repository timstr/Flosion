#include <Flosion/UI/Objects/AudioClip.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

#include <GUI/Helpers/CallbackButton.hpp>
#ifndef __linux__
#include <Flosion/Util/FileBrowser.hpp>
#endif

namespace flui {

    AudioClip::AudioClip(){
        // TODO
        addToRight(makePeg(&m_audioClip));


        auto bod = std::make_unique<ui::Boxed<ui::VerticalList>>();
        m_label = &bod->push_back<ui::Text>("AudioClip", getFont());
        bod->push_back<ui::CallbackButton>(
            "Load",
            getFont(),
            [&](){
#ifndef __linux__
                auto p = openFileDialog(L"Sound Files\0*.wav;*.ogg;*.flac\0");
                if (p.size() > 0){
                    loadFromFile(p);
                }
#endif
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

    RegisterFactoryObject(AudioClip, "AudioClip");

} // namespace flui
