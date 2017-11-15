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

	/*
	better name?
	validation function?
	on-change callback?
	on-return callback?
	*/
	struct TextEntryHelper : TextEntry {
		TextEntryHelper(const std::string& str, const sf::Font& font,
					const std::function<void(const std::string&)>& _on_return,
					const std::function<bool(const std::string&)>& _validate = {},
					const std::function<void(const std::string&)>& _on_change = {})
			: TextEntry(str, font), on_return(_on_return), validate(_validate), on_change(_on_change) {

			setBackGroundColor(sf::Color(0xFFFFFFFF));
			setTextColor(sf::Color(0xFF));
		}

		void onReturn(const std::string& entered_text) override {
			if (on_return && (!validate || validate(entered_text))){
				on_return(entered_text);
			}
		}

		void onType(const std::string& full_text) override {
			if (validate){
				if (validate(full_text)){
					setBackGroundColor(sf::Color(0xFFFFFFFF));
				} else {
					setBackGroundColor(sf::Color(0xFF8080FF));
				}
			}
			if (on_change){
				on_change(full_text);
			}
		}

		private:
		std::function<void(const std::string&)> on_return;
		std::function<bool(const std::string&)> validate;
		std::function<void(const std::string&)> on_change;
	};
}