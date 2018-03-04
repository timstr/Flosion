#pragma once

#include <gui/gui.h>
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
	struct Object : ui::Window {
		Object();

		Box* getBox();

		private:

		// TODO: hide invalid inputs/outputs
		void showNumberInputList(NumberWire* wire, vec2 pos);
		void showNumberOutputList(NumberWire* wire, vec2 pos);
		void showSoundInputList(SoundWire* wire, vec2 pos);
		void showSoundOutputList(SoundWire* wire, vec2 pos);

		void showWireDropList(const std::vector<std::pair<std::function<void()>, std::string>>& items, vec2 pos);

		// the FUI container that holds the current object
		// or if this is the master container should this ever be null
		Box* container;
		std::vector<SoundInput*> sound_inputs;
		std::vector<SoundOutput*> sound_outputs;
		std::vector<NumberInput*> number_inputs;
		std::vector<NumberOutput*> number_outputs;

		friend struct Box;
		friend struct SoundInput;
		friend struct SoundOutput;
		friend struct NumberInput;
		friend struct NumberOutput;

		friend struct ProcessingObject;
	};

} // namespace fui