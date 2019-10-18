#pragma once

#include <GUI/GUI.hpp>
#include "objectUI.h"
#include "WireBase.h"
#include "SoundSource.h"
#include "SoundInput.h"

namespace flui {

	struct SoundInput : WireInputBase<flo::SoundSource, flo::SoundInput> {
		SoundInput(ui::Ref<Object> parent_object, flo::SoundInput& target, std::string label) :
			WireInputBase(parent_object, target),
				m_label(label) {

			parent_object->addSoundInput(thisAs<SoundInput>());

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x00FF00FF));
			setMargin(5);
		}
		~SoundInput(){
			if (auto p = parentObject()){
				p->removeSoundInput(*this);
			}
		}

		const std::string& label() const {
			return m_label;
		}

	private:

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;

		const std::string m_label;
	};

	struct SoundOutput : WireOutputBase<flo::SoundSource, flo::SoundInput> {
		SoundOutput(ui::Ref<Object> parent_object, flo::SoundSource& target, std::string label) :
			WireOutputBase(parent_object, target),
			m_label(label) {

			parent_object->addSoundOutput(thisAs<SoundOutput>());

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x008000FF));
			setMargin(5);
		}
		~SoundOutput(){
			if (auto p = parentObject()){
				p->removeSoundOutput(*this);
			}
		}

		const std::string& label() const {
			return m_label;
		}

	private:

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;

		const std::string m_label;
	};

	struct SoundWire : WireBase<flo::SoundSource, flo::SoundInput> {
		SoundWire(ui::Ref<Box> parent_box) : 
			WireBase(parent_box) {
			
			parent_box->addSoundWire(thisAs<SoundWire>());
		}
		~SoundWire(){
			if (auto pb = parentBox()) {
				pb->removeSoundWire(*this);
			}
		}

		void onConnect(flo::SoundSource& src, flo::SoundInput& dst) override {
			dst.setSource(&src);
		}

		void onDisconnect(flo::SoundSource& src, flo::SoundInput& dst) override {
			dst.setSource(nullptr);
		}
	};

} // namespace flui
