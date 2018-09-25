#pragma once

#include <GUI/GUI.hpp>
#include "objectUI.h"

namespace fui {
	
	// Box is what holds all objects
	struct Box : Object {
		Box();

		void addObject(ui::Ref<Object> object);

		void releaseObject(ui::Ref<Object> object);

		void addSoundWire(ui::Ref<SoundWire> wire);

		void removeSoundWire(SoundWire& wire);

		void addNumberWire(ui::Ref<NumberWire> wire);

		void removeNumberWire(NumberWire& wire);

	private:

		bool onLeftClick(int clicks) override;

		struct InputPanel;

		std::vector<ui::Ref<Object>> m_objects;
		std::vector<ui::Ref<NumberWire>> m_numberwires;
		std::vector<ui::Ref<SoundWire>> m_soundwires;
		ui::Ref<FreeElement> m_object_container;
		ui::Ref<FreeElement> m_wire_container;
	};

	// allows convenient clustering of objects and collapsing into a more compact view
	struct CollapsibleBox : Box {
	
		void collapse();

		void expand();

		// TODO: put wire inputs/outputs on the edges of collapsible boxes
		// for wires leaving and entering the box
	};

	// MainBox is the singleton-intended container that holds everything in the current work area
	struct MainBox : Box {
		MainBox();

	};

} // namespace fui