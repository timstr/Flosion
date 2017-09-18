#pragma once
#include "gui.h"

namespace ui {

	// TODO:
	// callback button
	// slider
	// convenient text entry panel
	// convenient forms
	// tab navigation, space/enter toggling, automatic alignment and sizing

	struct CallbackButton : ui::Window {
		/*
		generic/custom rendering
		*/
	};

	struct RadioButton : ui::Window {
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

	struct Form : ui::Window {
		/*
		automatic content alignment?
		*/
	};
}