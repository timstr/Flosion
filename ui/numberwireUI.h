#pragma once

#include <GUI/GUI.hpp>
#include "objectUI.h"
#include "WireBase.h"
#include "NumberInput.h"

namespace fui {
	
	struct NumberInput : WireInputBase<musical::NumberSource, musical::NumberInput> {
		NumberInput(ui::WeakRef<Object> parent_object, musical::NumberInput& target, std::string label) :
			WireInputBase(parent_object, target),
			m_label(label) {

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x0000FFFF));
		}

		const std::string& label() const {
			return m_label;
		}

	private:

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;

		const std::string m_label;
	};

	struct NumberOutput : WireOutputBase<musical::NumberSource, musical::NumberInput> {
		NumberOutput(ui::WeakRef<Object> parent_object, musical::NumberSource& _target, std::string label) :
			WireOutputBase(parent_object, _target),
			m_label(label) {

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x000080FF));
		}

		const std::string& label() const {
			return m_label;
		}

	private:

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;

		const std::string m_label;
	};

	struct NumberWire : WireBase<musical::NumberSource, musical::NumberInput> {
		NumberWire(ui::WeakRef<Box> parent_box) :
			WireBase(parent_box) {

		}

		void onConnect(musical::NumberSource& src, musical::NumberInput& dst) override {
			dst.setSource(&src);
		}

		void onDisconnect(musical::NumberSource& src, musical::NumberInput& dst) override {
			dst.setSource(nullptr);
		}
	};


} // namespace fui