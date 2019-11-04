#pragma once

#include <GUI/GUI.hpp>

#include <Flosion/Core/SoundInput.hpp>
#include <Flosion/Core/NumberNode.hpp>
#include <Flosion/UI/Core/NumberPegs.hpp>
#include <Flosion/UI/Core/SoundPegs.hpp>

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
        ~Object();

		Box* getParentBox();

    protected:
        // Constructs a peg for the given number/sound input/output, with a desired label
        std::unique_ptr<NumberInputPeg> makeNumberInput(flo::NumberInput* ni, ui::String label = "Number Input");
        std::unique_ptr<NumberOutputPeg> makeNumberOutput(flo::NumberSource* no, ui::String label = "Number Output");
        std::unique_ptr<SoundInputPeg> makeSoundInput(flo::SoundInput* si, ui::String label = "Sound Input");
        std::unique_ptr<SoundOutputPeg> makeSoundOutput(flo::SoundSource* so, ui::String label = "Sound Output");

        std::unique_ptr<ui::Element> makeSimpleBody(ui::String caption, ui::Color color);
        
        const std::vector<NumberInputPeg*>& getNumberInputPegs();
        const std::vector<NumberOutputPeg*>& getNumberOutputPegs();
        const std::vector<SoundInputPeg*>& getSoundInputPegs();
        const std::vector<SoundOutputPeg*>& getSoundOutputPegs();

    protected:
        void addToLeft(std::unique_ptr<ui::Element>);
        void addToTop(std::unique_ptr<ui::Element>);
        void addtoRight(std::unique_ptr<ui::Element>);

        void addDragButton();

        void setBody(std::unique_ptr<ui::Element>);

		void showList(const std::vector<std::pair<std::string, std::function<void()>>>& items, ui::vec2 pos);

    private:
		bool onLeftClick(int) override;

		void onLeftRelease() override;

		void onDrag() override;

		void updateWires();

		// TODO: hide invalid inputs/outputs
		//void showNumberInputList(NumberWire* wire, ui::vec2 pos);
		//void showNumberOutputList(NumberWire* wire, ui::vec2 pos);
		//void showSoundInputList(SoundWire* wire, ui::vec2 pos);
		//void showSoundOutputList(SoundWire* wire, ui::vec2 pos);

		Box* m_parentBox;

		friend class Box;

    private:
        ui::VerticalList& m_leftContainer;
        ui::HorizontalList& m_topContainer;
        ui::VerticalList& m_rightContainer;

    private:
		std::vector<SoundInputPeg*> m_soundInputs;
		std::vector<SoundOutputPeg*> m_soundOutputs;
		std::vector<NumberInputPeg*> m_numberInputs;
		std::vector<NumberOutputPeg*> m_numberOutputs;
        
        void addSoundInputPeg(SoundInputPeg*);
        void removeSoundInputPeg(const SoundInputPeg*);
        void addSoundOutputPeg(SoundOutputPeg*);
        void removeSoundOutputPeg(const SoundOutputPeg*);
        void addNumberInputPeg(NumberInputPeg*);
        void removeNumberInputPeg(const NumberInputPeg*);
        void addNumberOutputPeg(NumberOutputPeg*);
        void removeNumberOutputPeg(const NumberOutputPeg*);

        friend class SoundInputPeg;
        friend class SoundOutputPeg;
        friend class NumberInputPeg;
        friend class NumberOutputPeg;
	};

} // namespace flui