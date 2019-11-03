#include <Flosion/UI/Core/BoxContextMenu.hpp>

#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

#include <GUI/Helpers/CallbackTextEntry.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

#include <cctype>

namespace flui {
    
    // TODO: this is duplicated from ObjectFactory.cpp
    namespace {
        void makeLowerCase(std::string& str) {
			std::transform(
			    str.begin(),
			    str.end(),
			    str.begin(),
			    [](uint32_t c){
                    if (c > std::numeric_limits<unsigned char>::max()){
                        return c;
                    }
                    return static_cast<uint32_t>(std::tolower(static_cast<int>(c)));
                }
		    );
		}

        std::pair<std::string, std::string> splitArgs(const std::string& s){
            auto n = s.find_first_not_of(" \t", 0);
            if (n == std::string::npos){
                return {"", ""};
            }
            n = s.find_first_of(" \t", n);
            if (n == std::string::npos){
                return {s, ""};
            }
            auto name = s.substr(0, n);
            n = s.find_first_not_of(" \t", n);
            if (n == std::string::npos){
                return {name, ""};
            }
            auto args = s.substr(n);
            return {std::move(name), std::move(args)};
        }
    }

    BoxContextMenu::BoxContextMenu(Box& parent)
        : m_parent(parent)
        , m_results(add<ui::VerticalList>(
            ui::FreeContainer::InsideLeft,
            ui::FreeContainer::OutsideBottom
        ))
        , m_textEntry(add<ui::CallbackTextEntry>(
            ui::FreeContainer::Center,
            ui::FreeContainer::Center,
            getFont(),
            [&](const ui::String& s){ handleSubmit(); },
            [&](const ui::String&){ return true; },
            [&](const ui::String& s){ refreshResults(s); }
        )) {

        /*m_textEntry.setBackgroundColor(0xFF);
        m_textEntry.setFillColor(0xFFFFFFFF);
        m_textEntry.setBorderColor(0x404040FF);
        m_textEntry.setBorderRadius(1.0f);
        m_textEntry.setMinWidth(100.0f);*/
    }

    void BoxContextMenu::startTyping(){
        m_textEntry.startTyping();
    }

    void BoxContextMenu::onLoseFocus(){
        close();
    }

    void BoxContextMenu::refreshResults(const ui::String& str){
        m_results.clear();
        m_results.setSize({0.0f, 0.0f});
        m_topResult = {};
        
        const auto [textEntered, args] = splitArgs(str.toAnsiString());

        m_args = args;

        const auto maxResults = std::size_t{10};

        // TODO: fuzzy matching
        // First results: all results with matching prefixes
        // Other results: all results with matching first letter and matching subsequence

        const auto match = [&](const std::string& s){
            if (s.size() < textEntered.size()){
                return false;
            }
            auto ss = s.substr(0, textEntered.size());
            makeLowerCase(ss);
            return ss == textEntered;
        };

        auto count = std::size_t{0};

        const auto& objCreators = Factory::getObjectCreators();
            
        for (auto it = objCreators.begin(), end = objCreators.end(); it != end && count < maxResults; ++it){
            if (match(it->first)){
                if (count == 0){
                    m_topResult = it->second;
                }

                m_results.push_back<ui::CallbackButton>(
                    it->first,
                    getFont(),
                    [this, &fn = it->second](){
                        addAndClose(fn(m_args));
                    }
                );
                ++count;
            }
        }
    }

    void BoxContextMenu::handleSubmit(){
        if (m_topResult){
            addAndClose(m_topResult(m_args));
        } else {
            close();
        }
    }

    void BoxContextMenu::addAndClose(std::unique_ptr<Object> obj){
        obj->setPos(pos() + (size() - obj->size()) * 0.5f);
        m_parent.addObject(std::move(obj));
        close();
    }

} // namespace flui
