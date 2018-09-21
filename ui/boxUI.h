#pragma once

#include <GUI/GUI.hpp>
#include "objectUI.h"

namespace fui {
	
	// Box is what holds all objects
	struct Box : Object {

		bool onLeftClick(int clicks) override;

		void addObject(ui::Ref<Object> object);

		void releaseObject(ui::Ref<Object> object);

		// TODO: does a box need to be aware of its contained wires?

	private:

		std::vector<ui::Ref<Object>> m_objects;
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