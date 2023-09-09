#pragma once

#include <GUI/Element.hpp>
#include <GUI/GUI.hpp>
#include "Font.h"

namespace fui {

	struct NumberInput;
	struct NumberOutput;
	struct NumberWire;
	struct SoundInput;
	struct SoundOutput;
	struct SoundWire;
	struct Box;

	// Object is the base class to all gui components representing sound and
	// numerical processing, and their related components
	struct Object : ui::FreeElement {
		Object();

		bool onLeftClick(int clicks) override;

		void onLeftRelease() override;

		ui::Ref<Box> getParentBox();

		void addSoundInput(ui::Ref<SoundInput> si);
		void removeSoundInput(SoundInput& si);

		void addSoundOutput(ui::Ref<SoundOutput> si);
		void removeSoundOutput(SoundOutput& si);

		void addNumberInput(ui::Ref<NumberInput> si);
		void removeNumberInput(NumberInput& si);

		void addNumberOutput(ui::Ref<NumberOutput> si);
		void removeNumberOutput(NumberOutput& si);

	private:

		void onDrag() override;

		void updateWires();

		// TODO: hide invalid inputs/outputs
		void showNumberInputList(ui::WeakRef<NumberWire> wire, vec2 pos);
		void showNumberOutputList(ui::WeakRef<NumberWire> wire, vec2 pos);
		void showSoundInputList(ui::WeakRef<SoundWire> wire, vec2 pos);
		void showSoundOutputList(ui::WeakRef<SoundWire> wire, vec2 pos);

		void showWireDropList(const std::vector<std::pair<std::string, std::function<void()>>>& items, vec2 pos);

		// the FUI container that holds the current object
		// or if this is the master container should this ever be null
		ui::WeakRef<Box> m_parentbox;
		std::vector<ui::WeakRef<SoundInput>> sound_inputs;
		std::vector<ui::WeakRef<SoundOutput>> sound_outputs;
		std::vector<ui::WeakRef<NumberInput>> number_inputs;
		std::vector<ui::WeakRef<NumberOutput>> number_outputs;

		friend struct Box;
	};

} // namespace fui