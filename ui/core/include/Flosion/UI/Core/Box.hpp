#pragma once

#include <GUI/GUI.hpp>

namespace flui {

    class Object;
    class NumberWire;
    class SoundWire;
	
	// Box is what holds all objects
	class Box : public ui::FreeContainer, public ui::Control, public ui::BoxElement {
    public:
		Box();

        // TODO

		/*void addObject(std::unique_ptr<Object> object);

		void releaseObject(ui::Ref<Object> object);

		void addSoundWire(ui::Ref<SoundWire> wire);

		void removeSoundWire(SoundWire& wire);

		void addNumberWire(ui::Ref<NumberWire> wire);

		void removeNumberWire(NumberWire& wire);*/

	private:

        void render(sf::RenderWindow& rw) override;

		std::vector<Object*> m_objects;
		std::vector<NumberWire*> m_numberwires;
		std::vector<SoundWire*> m_soundwires;
		//ui::Ref<FreeElement> m_object_container;
		//ui::Ref<FreeElement> m_wire_container;

		// calculate the bounding box of all contents
		// returns { top-left, bottom-right } as positions
		// in local space
		//std::pair<ui::vec2, ui::vec2> getContentBounds();

		// move all objects and wires by delta
		//void shiftContents(vec2 delta);

	private:

		bool onLeftClick(int) override;
	};

    // TODO
	// allows convenient clustering of objects and collapsing into a more compact view
	/*struct CollapsibleBox : Object {
		CollapsibleBox();

		void collapse();

		void expand();

		// TODO: make inner Box resize to fit contents and reposition
		// self as needed

		// TODO: put wire inputs/outputs on the edges of collapsible boxes
		// for wires leaving and entering the box

	private:

		void render(sf::RenderWindow& rw) override;

		bool onLeftClick(int clicks) override;

		void onLeftRelease() override;

		bool m_iscollapsed;

		ui::Ref<ui::Element> m_titlebar;
		ui::Ref<Box> m_box;
	};
	RegisterFactoryObject(CollapsibleBox, "box", "container");*/

	class MainBox : public Box {
    public:
		MainBox();
	};

} // namespace flui