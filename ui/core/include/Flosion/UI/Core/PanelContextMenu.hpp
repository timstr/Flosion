#pragma once

#include <GUI/GUI.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    class Panel;

    // PanelContextMenu is the text field and list of search results that
    // appears when one double-clicks a box
    class PanelContextMenu : public ui::FreeContainer, public ui::Control {
    public:
        PanelContextMenu(Panel& parent);

        void startTyping();

    private:
        void onLoseFocus() override;

        void refreshResults(const ui::String&);

        void handleSubmit();

        void addAndClose(std::unique_ptr<Object>);

        Panel& m_parent;
        ui::VerticalList& m_results;
        ui::TextEntry& m_textEntry;
        std::string m_objectName;
        std::vector<std::string> m_args;
    };

}
