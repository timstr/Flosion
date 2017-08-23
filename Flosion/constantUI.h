#pragma once

#include "FlosionUI.h"
#include "musical.h"

namespace fui {

	struct ConstantObject : ProcessingObject {
		ConstantObject(double value = 1){
			size = {100, 30};
			addChildWindow(new ui::Text(std::to_string(value), getFont()), {0, 0});
			addNumberOutput(new NumberOutput(&constant, this, "Value", {70, 0}));
			constant.setValue(value);
		}

		private:
		musical::Constant constant;
	};
	fuiRegisterObject(ConstantObject, "const", "constant", "value", "val", "number", "num");


	struct SliderObject : ProcessingObject {
		SliderObject(double minval, double maxval) : SliderObject() {
			min_value = minval;
			max_value = maxval;
		}
		SliderObject(){
			size = {300, 30};
			addChildWindow(slider = new Slider(this));
			addChildWindow(caption = new ui::Text("-", fui::getFont()));
			addNumberOutput(new NumberOutput(&value, this, "Value", {size.x, 0}));
		}

		void onLeftClick(int clicks) override {
			slider->jumpToCursor();
		}
		void onRightClick(int clicks) override {
			startDrag();
		}

		double min_value = 0;
		double max_value = 10;
		
		//TODO: add 'tightness' parameter to control exponential decay rate?
		struct SlidingConstant : musical::PureFunction {

			void updateValue(double newval, long double expected_duration){
				val1 = evaluate(nullptr);
				val2 = newval;
				timestamp1 = ui::getProgramTime();
				timestamp2 = timestamp1 + expected_duration;
			}

			float evaluate(musical::State* state) const override {
				// TODO: sliding bypassed for speed testing
				return val2;

				/* 
				long double now = ui::getProgramTime();

				const double rate = 0.04;

				// exponential decay approach
				// the '7' corresponds to a within-0.1% cutoff
				if ((now - timestamp1) / (timestamp2 - timestamp1) > 7 / rate){
					return val2;
				}
				return val1 + (val2 - val1) * (1 - exp(-rate * (now - timestamp1) / (timestamp2 - timestamp1)));
				*/
			};

			private:
			long double timestamp1, timestamp2;
			double val1, val2;
		} value;

		private:

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
				double val = parent->min_value + (parent->max_value - parent->min_value) * (pos.x / (parent->size.x - size.x));
				parent->value.updateValue(val, 0.001);
				parent->caption->setText(std::to_string(val));
				timestamp = ui::getProgramTime();
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

				long double now = ui::getProgramTime();

				double val = parent->min_value + (parent->max_value - parent->min_value) * (pos.x / (parent->size.x - size.x));

				parent->value.updateValue(val, now - timestamp);
				parent->caption->setText(std::to_string(val));

				timestamp = now;
			}
			SliderObject* parent;
			long double timestamp = 0;
		} *slider;
		ui::Text* caption;
	};
	fuiRegisterObject(SliderObject, "slider", "var", "variable");
};