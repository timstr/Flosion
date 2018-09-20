#pragma once

#include <GUI/GUI.hpp>
#include "objectUI.h"
#include "WireBase.h"
#include "NumberInput.h"

namespace fui {
	
	struct NumberInput : WireInputBase<musical::NumberSource, musical::NumberInput> {
		NumberInput(ui::WeakRef<Box> parent_box, musical::NumberInput& target) :
			WireInputBase(parent_box, target) {

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x0000FFFF));
		}

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;
	};

	struct NumberOutput : WireOutputBase<musical::NumberSource, musical::NumberInput> {
		NumberOutput(ui::WeakRef<Box> parent_box, musical::NumberSource& _target) :
			WireOutputBase(parent_box, _target) {

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x000080FF));
		}

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;
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

	ui::Ref<NumberInput::WireType> NumberInput::createWire(ui::Ref<Box> box){
		return ui::create<NumberWire>(box);
	}

	ui::Ref<NumberOutput::WireType> NumberOutput::createWire(ui::Ref<Box> box){
		return ui::create<NumberWire>(box);
	}


} // namespace fui