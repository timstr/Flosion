#pragma once
#include "gui.h"

namespace ui {

	// TODO:
	// callback button
	// slider
	// convenient text entry panel
	// binding of components to objects
	// tab navigation, space/enter toggling, automatic alignment and sizing

	struct CallbackButton : ui::Window {
		/*
		generic/custom rendering
		*/
	};

	struct ToggleButton : ui::Window {
		/*
		on-change callback?
		*/
	};

	struct PullDownMenu : ui::Window {
		/*
		on-change callback?
		*/
	};

	struct Slider : ui::Window {
		/*
		vertical/horizontal?
		generic/custom rendering?
		on-change callback?
		*/
	};

	struct TextInputEx : TextEntry {
		/*
		better name?
		validation function?
		on-change callback?
		on-return callback?
		*/
	};
}