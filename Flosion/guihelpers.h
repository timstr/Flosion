#pragma once
#include "gui.h"
#include "stringhelpers.h"

namespace ui {
	namespace helpers {

		// TODO:
		// callback button
		// slider
		// convenient text entry panel
		// binding of components to objects
		// tab navigation, space/enter toggling, automatic alignment and sizing

		struct CallbackButton : Window {
		
		};

		struct ToggleButton : Window {
			ToggleButton(bool& _target, unsigned int _size = 20) : target(_target) {
				size = vec2(_size, _size);
			}

			void onLeftClick(int clicks) override {
				target = !target;
			}

			void render(sf::RenderWindow& rw) override {
				sf::CircleShape circle;
				circle.setRadius(size.x / 2);
				circle.setFillColor(sf::Color(0xFFFFFFFF));
				rw.draw(circle);
				if (target){
					circle.setFillColor(sf::Color(0xFF));
					circle.setRadius(size.x * 0.45);
					circle.setPosition(vec2(size.x * 0.05, size.x * 0.05));
					rw.draw(circle);
				}
			}

			private:
			bool& target;
		};

		struct PullDownMenu : Window {
			/*
			on-change callback?
			*/
		};

		struct Slider : Window {
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

		struct NumberTextEntry : TextEntry {
			NumberTextEntry(double default, double _min, double _max, const std::function<void(double)>& _callback, const sf::Font& font)
				: TextEntry(toString(default), font), value(default), min(_min), max(_max), callback(_callback) {
				setBackGroundColor(sf::Color(0xFFFFFFFF));
				setTextColor(sf::Color(0xFF));
			}

			void onReturn(const std::string& entered_text) override {
				if (validate(entered_text) && callback){
					double val = stringToDouble(entered_text);
					callback(val);
				}
			}

			void onType(const std::string& full_text) override {
				if (validate(full_text)){
					setBackGroundColor(sf::Color(0xFFFFFFFF));
				} else {
					setBackGroundColor(sf::Color(0xFF8080FF));
				}
			}

			private:
			double value;
			const double min;
			const double max;
			std::function<void(double)> callback;

			bool validate(const std::string& text){
				double val = stringToDouble(text);
				return !std::isnan(val);
			}
		};
	
		template<typename Model>
		struct Form : ui::Window {
			Form(){

			}
		};
	}
}