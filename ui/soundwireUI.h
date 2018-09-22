#pragma once

#include <GUI/GUI.hpp>
#include "objectUI.h"
#include "WireBase.h"
#include "SoundSource.h"
#include "SoundInput.h"

namespace fui {

	struct SoundInput : WireInputBase<musical::SoundSource, musical::SoundInput> {
		SoundInput(ui::WeakRef<Object> parent_object, musical::SoundInput& target, std::string label) :
			WireInputBase(parent_object, target),
				m_label(label) {
			
			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x00FF00FF));
			setMargin(5);
		}

		const std::string& label() const {
			return m_label;
		}

	private:

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;

		const std::string m_label;
	};

	struct SoundOutput : WireOutputBase<musical::SoundSource, musical::SoundInput> {
		SoundOutput(ui::WeakRef<Object> parent_object, musical::SoundSource& target, std::string label) :
			WireOutputBase(parent_object, target),
			m_label(label) {

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x008000FF));
			setMargin(5);
		}

		const std::string& label() const {
			return m_label;
		}

	private:

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;

		const std::string m_label;
	};

	struct SoundWire : WireBase<musical::SoundSource, musical::SoundInput> {
		SoundWire(ui::WeakRef<Box> parent_box) : 
			WireBase(parent_box) {
			
		}

		void onConnect(musical::SoundSource& src, musical::SoundInput& dst) override {
			dst.setSource(&src);
		}

		void onDisconnect(musical::SoundSource& src, musical::SoundInput& dst) override {
			dst.setSource(nullptr);
		}
	};

} // namespace fui