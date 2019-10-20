#pragma once

#include <GUI/GUI.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    class Box;

    // BoxContextMenu is the text field and list of search results that
    // appears when one double-clicks a box
    class BoxContextMenu : public ui::FreeContainer, public ui::Control {
    public:
        BoxContextMenu(Box& parent);

        void startTyping();

    private:
        void onLoseFocus() override;

        void refreshResults(const ui::String&);

        void handleSubmit();

        void addAndClose(std::unique_ptr<Object>);

        Box& m_parent;
        ui::GridContainer& m_results;
        ui::TextEntry& m_textEntry;
        Factory::ObjectCreator m_topResult;
    };

}
