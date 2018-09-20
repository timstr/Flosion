#pragma once

#include "musical.h"
#include "FlosionUI.h"
#include "Font.h"
#include <gui/forms.h>

// TODO: remove
#include <iostream>

namespace fui {

	struct ConstantObject : ProcessingObject {
		ConstantObject(float value = 1.0f){
			size = {100, 30};
			addChildWindow(caption = new ui::Text(toString(value), getFont()));
			addChildWindow(new NumberOutput(&constant, this, "Value"), rightOf(this));
			constant.setValue(value);
		}

		void onLeftClick(int clicks) override {
			showMenu(localMousePos());
		}

		void updateCaption(){
			caption->setText(toString(constant.getValue()));
		}

		void showMenu(vec2 center){
			ui::forms::Model model;
			model["Name"] = std::string("your name here");
			model["Value"] = constant.getValue();
			model["Pease Porridge"] = ui::forms::PullDownProperty<int>({
				{1, "Hot"},
				{2, "Cold"},
				{3, "In the pot, nine days old"}
			}, 0);

			ui::forms::Form* form = new ui::forms::Form(model, getFont());

			form->onSubmit([this](ui::forms::Model m){
				std::cout << "Name: " << (std::string)m["Name"] << std::endl;
				std::cout << "Value: " << (float)m["Value"] << std::endl;
				std::cout << "Pease Porridge: " << (int)m["Pease Porridge"] << std::endl;

				/*float val = m["Value"];
				if (!std::isnan(val)){
					constant.setValue(val);
					updateCaption();
				}*/
			});


			addChildWindow(form);
			form->pos = center - form->size * 0.5f;
			form->pos = vec2(floor(form->pos.x), floor(form->pos.y));
		}

		private:
		musical::Constant constant;
		ui::Text* caption;
	};
	fuiRegisterObject(ConstantObject, "constant", "value", "number");

	// TODO: add menu similar to spline menu
	struct SliderObject : ProcessingObject {
		SliderObject(){
			size = {300, 30};
			addChildWindow(slider = new Slider(this));
			addChildWindow(caption = new ui::Text("", fui::getFont()));

			NumberOutput* output = new NumberOutput(&value, this, "Value");

			addChildWindow(output, rightOf(this));

			updateCaption();
		}
		SliderObject(float minval, float maxval) : SliderObject() {
			setRange(minval, maxval);
		}

		void onLeftClick(int clicks) override {
			if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
				showMenu(localMousePos());
			} else {
				slider->jumpToCursor();
			}
		}
		void onRightClick(int clicks) override {
			startDrag();
		}
		
		void setRange(float minimum, float maximum){
			min_value = std::min(minimum, maximum);
			max_value = std::max(minimum, maximum);
			value.setValue(std::min(std::max(value.getValue(), min_value), max_value));
			updateCaption();
			updateSlider();
		}

		void updateSlider(){
			slider->pos.y = 0.0f;
			slider->pos.x = (size.x - slider->size.x) * ((value.getValue() - min_value) / (max_value - min_value));
		}

		void showMenu(vec2 center){
			ui::forms::Model model;
			model["Minimum"] = min_value;
			model["Maximum"] = max_value;
			model["Value"] = value.getValue();
			model["Name"] = std::string("your name here");

			ui::forms::Form* form = new ui::forms::Form(model, getFont());

			form->onSubmit([this](ui::forms::Model m){
				value.setValue(m["Value"]);
				setRange(m["Minimum"], m["Maximum"]);
			});

			addChildWindow(form, center - form->size * 0.5f);
			form->pos = vec2(floor(form->pos.x), floor(form->pos.y));
		}


		private:

		musical::Constant value;

		float min_value = 0.0f;
		float max_value = 10.0f;

		void updateCaption(){
			caption->setText(toString(value.getValue()));
		}

		struct Slider : ui::Window {
			Slider(SliderObject* _parent){
				parent = _parent;
				size = {30, 30};
			}
			void jumpToCursor(){
				pos.x = parent->localMousePos().x - (size.x / 2);
				if (pos.x < 0){
					pos.x = 0;
				}
				if (pos.x > parent->size.x - size.x){
					pos.x = parent->size.x - size.x;
				}
				float val = parent->min_value + (parent->max_value - parent->min_value) * (pos.x / (parent->size.x - size.x));
				parent->value.setValue(val);
				parent->updateCaption();
				startDrag();
			}
			void onLeftClick(int clicks) override {
				startDrag();
			}
			void onRightClick(int clicks) override {
				parent->startDrag();
			}
			void onDrag() override {
				pos.y = 0;
				if (pos.x < 0){
					pos.x = 0;
				}
				if (pos.x > parent->size.x - size.x){
					pos.x = parent->size.x - size.x;
				}

				float val = parent->min_value + (parent->max_value - parent->min_value) * (pos.x / (parent->size.x - size.x));

				parent->value.setValue(val);
				parent->updateCaption();
			}
			SliderObject* parent;
		} *slider;
		ui::Text* caption;
	};
	fuiRegisterObject(SliderObject, "slider", "variable");
};