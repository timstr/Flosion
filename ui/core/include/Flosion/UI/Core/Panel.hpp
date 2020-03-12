#pragma once

#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/NumberSource.hpp>
#include <GUI/GUI.hpp>

#include <map>

namespace flui {

    class Object;
    class NumberInputPeg;
    class NumberOutputPeg;
    class NumberWire;
    class SoundInputPeg;
    class SoundOutputPeg;
    class SoundWire;
    class BorrowingNumberObject;
	
    // TODO: ctrl-A to select everything

	// Panel is what holds all objects
	class Panel : public ui::Boxed<ui::FreeContainer>, public ui::Control {
    public:
		Panel();
        ~Panel();

        // TODO: it's not immediately clear what the difference is between this
        // and add<SomeObject>() or adopt(some_object_unique_ptr)
		void addObject(Object*);
        void removeObject(const Object*);
        
        // Adds a NumberWire to the panel.
        // At most one of src and dst may be null.
        // TODO: rename to createWire
        NumberWire* addWire(flo::NumberSource* src, flo::NumberInput* dst);
        void removeWire(NumberWire*);

        // Adds a SoundWire to the panel.
        // At most one of src and dst may be null.
        // TODO: rename to createWire
        SoundWire* addWire(flo::SoundSource* src, flo::SoundInput* dst);
        void removeWire(SoundWire*);

        std::vector<const Object*> getObjects() const noexcept;
        std::vector<const NumberWire*> getNumberWires() const noexcept;
        std::vector<const SoundWire*> getSoundWires() const noexcept;

        NumberInputPeg* findPegFor(const flo::NumberInput*);
        NumberOutputPeg* findPegFor(const flo::NumberSource*);
        SoundInputPeg* findPegFor(const flo::SoundInput*);
        SoundOutputPeg* findPegFor(const flo::SoundSource*);

        const NumberInputPeg* findPegFor(const flo::NumberInput*) const;
        const NumberOutputPeg* findPegFor(const flo::NumberSource*) const;
        const SoundInputPeg* findPegFor(const flo::SoundInput*) const;
        const SoundOutputPeg* findPegFor(const flo::SoundSource*) const;

        void showWarningAt(ui::vec2);

	private:

		std::vector<Object*> m_objects;
		std::vector<NumberWire*> m_numberwires;
		std::vector<SoundWire*> m_soundwires;

    private:

        BorrowingNumberObject* findBorrowerFor(const flo::BorrowingNumberSource*);

        template<typename Traits>
        friend class Wire;

        friend class SoundObject;

		// calculate the bounding box of all contents
		// returns { top-left, bottom-right } as positions
		// in local space
		//std::pair<ui::vec2, ui::vec2> getContentBounds();

		// move all objects and wires by delta
		//void shiftContents(vec2 delta);

	private:

        std::vector<Object*> findObjectsInRect(ui::vec2 topLeft, ui::vec2 size);

        void selectObjects(std::vector<Object*>);

        void clearSelection();

        void moveSelection(ui::vec2 delta);

        void deleteSelection();

        // Serializes all selected objects to the clipboard.
        // Does nothing if there is no selection.
        void copySelection();

        // Serializes all selected objects to the clipboard,
        // then removes them.
        // Does nothing if there is no selection.
        void cutSelection();

        // Deserializes objects from the clipboard and adds them
        // to the panel. Any selected objects are removed, and
        // the newly inserted objects are selected.
        // Does nothing if the clipboard does not contain valid data.
        void paste();

        class SelectedObjects : public ui::FreeContainer, public ui::Control, public ui::Draggable {
        public:
            SelectedObjects(Panel& parentPanel, const std::vector<Object*>&);

            void putObjectsBack();

            std::vector<const Object*> getObjects() const;

            void moveObjects(ui::vec2 delta);

            void deleteSelection();

            // TODO: add/remove objects to/from selection

            void deselectAll();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            bool onKeyDown(ui::Key) override;

            void onLoseFocus() override;

            void onDrag() override;

            void render(sf::RenderWindow&) override;

            class FrontCover : public ui::Control, public ui::BoxElement {
            public:
                FrontCover(SelectedObjects& parent);

            private:
                bool onLeftClick(int) override;

                SelectedObjects& m_parent;
            };

            Panel& m_parentPanel;

            // vector of (selected object, base position) pairs
            std::vector<std::pair<Object*, ui::vec2>> m_selectedObjects;
            ui::vec2 m_lastPos;
        };


        bool onKeyDown(ui::Key) override;

		bool onLeftClick(int) override;

        void onLeftRelease() override;

        void render(sf::RenderWindow&) override;

        std::optional<ui::vec2> m_selectionStart;
        ui::Draggable* m_selectionDragger;
        SelectedObjects* m_selection;
	};

    // TODO: collapsible panel

	class MainPanel : public Panel {
    public:
		MainPanel();
	};

} // namespace flui