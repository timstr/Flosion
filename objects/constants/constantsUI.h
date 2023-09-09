#pragma once

#include "Sample.h"
#include "ObjectFactory.h"
#include "Font.h"
#include <GUI/Helpers.hpp>
#include "NumberSource.h"

namespace fui {

	struct ConstantObject : Object {
		ConstantObject(){
			setMinSize({100, 30});
			m_caption = add<ui::Text>("", getFont());
			add<NumberOutput>(this->thisAs<Object>(), m_constant, "Value");
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

		flo::Constant m_constant;
		ui::Ref<ui::Text> m_caption;
	};
	RegisterFactoryObject(ConstantObject, "constant", "value", "number");

	// TODO: add menu
	struct SliderObject : Object {
		SliderObject(){
			auto onChange = [this](float v) {
				m_value.setValue(v);
			};

			m_slider = add<ui::Slider>((min_value + max_value) * 0.5f, min_value, max_value, getFont(), onChange);
			m_slider->setSize({ 200, 30 }, true);
			m_slider->setBorderRadius(15);
			m_slider->getHandle()->setBorderRadius(15);

			add<NumberOutput>(this->thisAs<Object>(), m_value, "Value");
		}
		SliderObject(float minval, float maxval) : SliderObject() {
			setRange(minval, maxval);
		}

		bool onLeftClick(int clicks) override {
			if (keyDown(sf::Keyboard::LShift) || keyDown(sf::Keyboard::RShift)){
				showMenu(localMousePos());
				return true;
			}
			return Object::onLeftClick(clicks);
		}
		
		void setRange(float minimum, float maximum){
			min_value = std::min(minimum, maximum);
			max_value = std::max(minimum, maximum);
			m_value.setValue(std::min(std::max(m_value.getValue(), min_value), max_value));
			updateSlider();
		}

		void updateSlider(){
			m_slider->setValue(m_value.getValue());
		}

		void showMenu(vec2 center){
			// TODO
		}


		private:

		float min_value = 0.0f;
		float max_value = 10.0f;


		ui::Ref<ui::Slider> m_slider;
		flo::Constant m_value;
	};
	RegisterFactoryObject(SliderObject, "slider", "variable");
};