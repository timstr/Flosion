#pragma once

#include <GUI/GUI.hpp>

#include <Flosion/UI/Core/Font.hpp>

#include <Flosion/Core/SoundInput.hpp>

namespace flui {

	class NumberInput;
	class NumberOutput;
	class NumberWire;
	class SoundInput;
	class SoundOutput;
	class SoundWire;
	class Box;

	// Object is the base class to all gui components representing sound and
	// numerical processing, and their related components
	class Object : public ui::GridContainer, public ui::Control, public ui::Draggable {
    public:
		Object();

		Box* getParentBox();

        SoundInput* addSoundInput(flo::SoundInput* si, ui::String label = "sound input");
        void removeSoundInput(const SoundInput* si);

        SoundOutput* addSoundOutput(flo::SoundSource* so, ui::String label = "sound output");
        void removeSoundOutput(const SoundOutput* so);

        NumberInput* addNumberInput(flo::NumberInput* ni, ui::String label = "number input");
        void removeNumberInput(const NumberInput* ni);

        NumberOutput* addNumberOutput(flo::NumberSource* no, ui::String label = "number output");
        void removeNumberOutput(const NumberOutput* no);

        /*
		void registerSoundInput(SoundInput* si);
		void unregisterSoundInput(const SoundInput* si);

		void registerSoundOutput(SoundOutput* si);
		void unregisterSoundOutput(const SoundOutput* si);

		void registerNumberInput(NumberInput* si);
		void unregisterNumberInput(const NumberInput* si);

		void registerNumberOutput(NumberOutput* si);
		void unregisterNumberOutput(const NumberOutput* si);*/

    private:
		bool onLeftClick(int) override;

		void onLeftRelease() override;

		void onDrag() override;

		void updateWires();

		// TODO: hide invalid inputs/outputs
		void showNumberInputList(NumberWire* wire, ui::vec2 pos);
		void showNumberOutputList(NumberWire* wire, ui::vec2 pos);
		void showSoundInputList(SoundWire* wire, ui::vec2 pos);
		void showSoundOutputList(SoundWire* wire, ui::vec2 pos);

		void showWireDropList(const std::vector<std::pair<std::string, std::function<void()>>>& items, ui::vec2 pos);

		Box* m_parentbox;
		std::vector<SoundInput*> sound_inputs;
		std::vector<SoundOutput*> sound_outputs;
		std::vector<NumberInput*> number_inputs;
		std::vector<NumberOutput*> number_outputs;

		friend class Box;
	};

} // namespace flui