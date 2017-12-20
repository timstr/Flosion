#pragma once

#include <type_traits>
#include "guiformscontrol.h"
#include "guiformsmodel.h"
#include "guihelpers.h"

namespace ui {
	namespace forms {

		struct Form : Window {
			Form(const Model& _model) : model(_model) {
				size = {0, 5};
				float y = 5;
				for (auto it = model.properties.begin(); it != model.properties.end(); it++){
					Text* caption = new Text(it->first, fui::getFont());
					Control* control = it->second->makeControl();
					controls.push_back(control);
					addChildWindow(caption, insideLeft(this, 5), y);
					addChildWindow(control, rightOf(caption, 5), y);
					size.x = std::max(size.x, caption->size.x + control->size.x + 15);
					size.y += control->size.y + 5;
					y += control->size.y + 5;
				}
				auto submitbtn = new ui::helpers::CallbackButton("Submit", [this](){
					this->submit();
					this->close();
				});
				size.x = std::max(size.x, submitbtn->size.x + 10);
				size.y += submitbtn->size.y + 5;
				addChildWindow(submitbtn, middleOfX(this), y);
			}

			void onSubmit(const std::function<void(Model)>& _on_submit){
				on_submit = _on_submit;
			}

			void submit(){
				for (Control* control: controls){
					control->submit();
				}
				if (on_submit){
					on_submit(model);
				}
			}

			private:

			Model model;
			std::function<void(Model)> on_submit;
			std::vector<Control*> controls;
			friend struct Control;
		};


		struct StringProperty : TypeProperty<std::string> {
			StringProperty(const std::string& str) : TypeProperty(str) {

			}

			Control* makeControl();
		};

		struct StringControl : Control {
			StringControl(StringProperty& _strprop) : strprop(_strprop) {
				text = new TextEntry(strprop.value, fui::getFont());
				addChildWindow(text);
				size = text->size;
			}

			void submit() override {
				strprop.value = text->getText();
			}

			void render(sf::RenderWindow& rw) override {
				size = text->size;
				renderChildWindows(rw);
			}

			private:
			TextEntry* text;
			StringProperty& strprop;
		};

		Control* StringProperty::makeControl(){
			return new StringControl(*this);
		}
	}
}