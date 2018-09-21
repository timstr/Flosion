#pragma once

#include "musical.h"
#include "FlosionUI.h"
#include "Font.h"
#include <GUI/Helpers.hpp>

// TODO: remove
#include <iostream>

namespace fui {

	struct ConstantObject : Object {
		ConstantObject(){
			setMinSize({100, 30});
			m_caption = add<ui::Text>("", getFont());
			add<NumberOutput>(thisAs<Object>(), m_constant, "Value");
			m_constant.setValue(0.0f);
			updateCaption();
		}

		void updateCaption(){
			m_caption->setText(std::to_string(m_constant.getValue()));
		}

		void showMenu(vec2 center){
			// TODO
		}

		float getValue() const {
			return m_constant.getValue();
		}

		void setValue(float value) {
			m_constant.setValue(value);
		}

	private:

		bool onLeftClick(int clicks) override {
			showMenu(localMousePos());
			return true;
		}

		musical::Constant m_constant;
		ui::Ref<ui::Text> m_caption;
	};
	RegisterFactoryObject(ConstantObject, "constant", "value", "number");

	// TODO: add menu
	struct SliderObject : Object {
		SliderObject(){
			setSize({300, 30}, true);
			m_slider = add<Slider>(*this);
			m_caption = add<ui::Text>("", fui::getFont());

			add<NumberOutput>(thisAs<Object>(), m_value, "Value");

			updateCaption();
		}
		SliderObject(float minval, float maxval) : SliderObject() {
			setRange(minval, maxval);
		}

		bool onLeftClick(int clicks) override {
			if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
				showMenu(localMousePos());
			} else {
				m_slider->jumpToCursor();
			}
			return true;
		}
		bool onRightClick(int clicks) override {
			startDrag();
			return true;
		}
		
		void setRange(float minimum, float maximum){
			min_value = std::min(minimum, maximum);
			max_value = std::max(minimum, maximum);
			m_value.setValue(std::min(std::max(m_value.getValue(), min_value), max_value));
			updateCaption();
			updateSlider();
		}

		void updateSlider(){
			m_slider->setTop(0.0f);
			m_slider->setLeft((width() - m_slider->width()) * ((m_value.getValue() - min_value) / (max_value - min_value)));
		}

		void showMenu(vec2 center){
			// TODO
		}


		private:

		float min_value = 0.0f;
		float max_value = 10.0f;

		void updateCaption(){
			m_caption->setText(std::to_string(m_value.getValue()));
		}

		struct Slider : ui::FreeElement {
			Slider(SliderObject& _parent) : parent(_parent) {
				setSize({30, 30}, true);
			}
			void jumpToCursor(){
				setLeft(parent.localMousePos().x - width() * 0.5f);
				if (left() < 0){
					setLeft(0);
				}
				if (left() > parent.width() - width()){
					setLeft(parent.width() - width());
				}
				float val = parent.min_value + (parent.max_value - parent.min_value) * (left() / (parent.width() - width()));
				parent.m_value.setValue(val);
				parent.updateCaption();
				startDrag();
			}
			bool onLeftClick(int clicks) override {
				startDrag();
				return true;
			}
			bool onRightClick(int clicks) override {
				parent.startDrag();
				return true;
			}
			void onDrag() override {
				setTop(0);
				if (left() < 0){
					setLeft(0);
				}
				if (left() > parent.width() - width()){
					setLeft(parent.width() - width());
				}

				float val = parent.min_value + (parent.max_value - parent.min_value) * (left() / (parent.width() - width()));

				parent.m_value.setValue(val);
				parent.updateCaption();
			}
			SliderObject& parent;
		};

		ui::Ref<Slider> m_slider;
		ui::Ref<ui::Text> m_caption;
		musical::Constant m_value;
	};
	RegisterFactoryObject(SliderObject, "slider", "variable");
};