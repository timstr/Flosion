#pragma once

#include <GUI/GUI.hpp>
#include "ObjectUI.h"
#include "ObjectFactory.h"

namespace fui {
	
    struct SoundWire;
    struct NumberWire;

	// Box is what holds all objects
	struct Box : ui::FreeElement {
		Box();

		void addObject(ui::Ref<Object> object);

		void releaseObject(ui::Ref<Object> object);

		void addSoundWire(ui::Ref<SoundWire> wire);

		void removeSoundWire(SoundWire& wire);

		void addNumberWire(ui::Ref<NumberWire> wire);

		void removeNumberWire(NumberWire& wire);

	protected:

		std::vector<ui::Ref<Object>> m_objects;
		std::vector<ui::Ref<NumberWire>> m_numberwires;
		std::vector<ui::Ref<SoundWire>> m_soundwires;
		ui::Ref<FreeElement> m_object_container;
		ui::Ref<FreeElement> m_wire_container;

		// calculate the bounding box of all contents
		// returns { top-left, bottom-right } as positions
		// in local space
		std::pair<vec2, vec2> getContentBounds();

		// move all objects and wires by delta
		void shiftContents(vec2 delta);

	private:

		bool onLeftClick(int clicks) override;

		struct InputPanel;

		friend struct CollapsibleBox;
	};

	// allows convenient clustering of objects and collapsing into a more compact view
	struct CollapsibleBox : Object {
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
	RegisterFactoryObject(CollapsibleBox, "box", "container");

	// MainBox is the singleton-intended container that holds everything in the current work area
	struct MainBox : Box {
		MainBox();

	private:
		void render(sf::RenderWindow& rw) override;

		void resizeToFit(vec2 screensize);
	};

} // namespace fui