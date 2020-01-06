#include <Flosion/UI/Core/PanelContextMenu.hpp>

#include <Flosion/UI/Core/Panel.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

#include <GUI/Helpers/CallbackTextEntry.hpp>
#include <GUI/Helpers/CallbackButton.hpp>

#include <cctype>
#include <iterator>
#include <sstream>

namespace flui {
    
    // TODO: this is duplicated from ObjectFactory.cpp
    namespace {
        void makeLowerCase(std::string& str) {
			std::transform(
			    str.begin(),
			    str.end(),
			    str.begin(),
			    [](int c){
                    return static_cast<char>(std::tolower(c));
                }
		    );
		}

        std::vector<std::string> split(const std::string& s){
            auto ss = std::istringstream{s};
            auto v = std::vector<std::string>{
                std::istream_iterator<std::string>{ss},
                std::istream_iterator<std::string>{}
            };
            if (v.size() > 0){
                makeLowerCase(v[0]);
            }
            return v;
        }
    }

    PanelContextMenu::PanelContextMenu(Panel& parent)
        : m_parent(parent)
        , m_results(add<ui::VerticalList>(
            ui::FreeContainer::InsideLeft,
            ui::FreeContainer::OutsideBottom
        ))
        , m_textEntry(add<ui::CallbackTextEntry>(
            ui::FreeContainer::Center,
            ui::FreeContainer::Center,
            getFont(),
            [&](const ui::String&){ handleSubmit(); },
            [&](const ui::String&){ return true; },
            [&](const ui::String& s){ refreshResults(s); }
        )) {

        /*m_textEntry.setBackgroundColor(0xFF);
        m_textEntry.setFillColor(0xFFFFFFFF);
        m_textEntry.setBorderColor(0x404040FF);
        m_textEntry.setBorderRadius(1.0f);
        m_textEntry.setMinWidth(100.0f);*/
    }

    void PanelContextMenu::startTyping(){
        m_textEntry.startTyping();
    }

    void PanelContextMenu::onLoseFocus(){
        close();
    }

    void PanelContextMenu::refreshResults(const ui::String& str){
        m_results.clear();
        m_results.setSize({0.0f, 0.0f});
        
        auto args = split(str.toAnsiString());

        if (args.size() == 0){
            return;
        }
        m_objectName = std::move(args.front());
        args.erase(args.begin());
        m_args = std::move(args);


        const auto maxResults = std::size_t{10};

        // TODO: fuzzy matching
        // First results: all results with matching prefixes
        // Other results: all results with matching first letter and matching subsequence

        const auto match = [&](const std::string& s){
            if (s.size() < m_objectName.size()){
                return false;
            }
            auto ss = s.substr(0, m_objectName.size());
            makeLowerCase(ss);
            return ss == m_objectName;
        };

        auto count = std::size_t{0};

        const auto& names = Factory::getObjectNames();
            
        for (auto it = names.begin(), end = names.end(); it != end && count < maxResults; ++it){
            if (match(*it)){
                if (count == 0){
                    m_objectName = *it;
                }

                m_results.push_back<ui::CallbackButton>(
                    *it,
                    getFont(),
                    [this, name =*it](){
                        addAndClose(Factory::createObject(name, m_args));
                    }
                );
                ++count;
            }
        }
    }

    void PanelContextMenu::handleSubmit(){
        if (m_objectName.size() > 0){
            auto obj = Factory::createObject(m_objectName, m_args);
            if (obj){
                addAndClose(std::move(obj));
                return;
            }
        }
        close();
    }

    void PanelContextMenu::addAndClose(std::unique_ptr<Object> obj){
        obj->setPos(pos() + (size() - obj->size()) * 0.5f);
        m_parent.addObject(obj.get());
        m_parent.adopt(std::move(obj));
        close();
    }

} // namespace flui
