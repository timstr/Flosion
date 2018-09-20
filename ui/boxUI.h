#pragma once

#include <GUI/GUI.hpp>
#include "objectUI.h"

namespace fui {
	
	// Box is what holds all objects
	struct Box : Object {

		bool onLeftClick(int clicks) override;

	private:



		std::vector<ui::Ref<Object>> m_objects;
	};

	// allows convenient clustering of objects and collapsing into a more compact view
	struct CollapsibleBox : Box {
	
		void collapse();

		void expand();

	};

	// MainBox is the singleton-intended container that holds everything in the current work area
	struct MainBox : Box {

		// TODO

	};

} // namespace fui