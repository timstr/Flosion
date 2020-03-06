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
    // Consider moving this to Util
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
        auto objName = std::move(args.front());
        makeLowerCase(objName);
        args.erase(args.begin());
        m_args = std::move(args);


        const auto maxResults = std::size_t{10};

        // TODO: fuzzy matching
        // First results: all results with matching prefixes
        // Other results: all results with matching first letter and matching subsequence

        const auto strictMatch = [&](const std::string& s){
            // Does the string start with the exact search term?
            if (s.size() < objName.size()){
                return false;
            }
            auto ss = s.substr(0, objName.size());
            makeLowerCase(ss);
            return ss == objName;
        };

        const auto fuzzyMatch = [&](const std::string& s) {
            // Does the string start with the first letter of the search term
            // and contain the rest of the search term as a subsequence?
            assert(objName.size() > 0);
            assert(s.size() > 0);
            auto ss = s;
            makeLowerCase(ss);
            auto itA = objName.begin();
            auto itB = ss.begin();
            if (*itA != *itB) {
                return false;
            }
            ++itA;
            ++itB;
            while (true) {
                if (itA == objName.end()) {
                    return true;
                }
                if (itB == ss.end()) {
                    break;
                }
                if (*itA == *itB) {
                    ++itA;
                }
                ++itB;
            }
            return false;
        };

        auto count = std::size_t{0};
        const auto pushResult = [&](const std::string& name) {
            if (count == 0){
                m_objectName = name;
            }

            m_results.push_back<ui::CallbackButton>(
                name,
                getFont(),
                [this, str = name](){
                    addAndClose(Factory::createObject(str, m_args));
                }
            );
            ++count;
            return count < maxResults;
        };


        auto names = Factory::getObjectNames();
        sort(begin(names), end(names));

        std::set<const std::string*> resultsSoFar;
        for (const auto& n : names) {
            if (strictMatch(n)) {
                if (!pushResult(n)) {
                    return;
                }
                resultsSoFar.insert(&n);
            }
        }
        
        for (const auto& n : names) {
            if (fuzzyMatch(n)) {
                if (resultsSoFar.count(&n) != 0) {
                    continue;
                }
                if (!pushResult(n)) {
                    return;
                }
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
