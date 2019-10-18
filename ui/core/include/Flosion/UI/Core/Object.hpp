#pragma once

#include <GUI/GUI.hpp>

#include <Flosion/Core/SoundInput.hpp>
#include <Flosion/Core/NumberNode.hpp>

namespace flui {

	class NumberInputPeg;
	class NumberOutputPeg;
	class NumberWire;
	class SoundInputPeg;
	class SoundOutputPeg;
	class SoundWire;
	class Box;

	// Object is the base class to all gui components representing sound and
	// numerical processing, and their related components
	class Object : public ui::GridContainer, public ui::Control, public ui::Draggable {
    public:
		Object();

		Box* getParentBox();

        // Constructs a peg for the given number/sound input/output, with a desired label
        SoundInputPeg* addSoundInput(flo::SoundInput* si, ui::String label = "sound input");
        SoundOutputPeg* addSoundOutput(flo::SoundSource* so, ui::String label = "sound output");
        NumberInputPeg* addNumberInput(flo::NumberInput* ni, ui::String label = "number input");
        NumberOutputPeg* addNumberOutput(flo::NumberSource* no, ui::String label = "number output");

        // Removes a previously added peg
        void removeSoundInput(const SoundInputPeg* si);
        void removeSoundOutput(const SoundOutputPeg* so);
        void removeNumberInput(const NumberInputPeg* ni);
        void removeNumberOutput(const NumberOutputPeg* no);


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

		void showList(const std::vector<std::pair<std::string, std::function<void()>>>& items, ui::vec2 pos);

		Box* m_parentbox;
		std::vector<SoundInputPeg*> m_soundInputs;
		std::vector<SoundOutputPeg*> m_soundOutputs;
		std::vector<NumberInputPeg*> m_numberInputs;
		std::vector<NumberOutputPeg*> m_numberOutputs;

		friend class Box;
	};

} // namespace flui