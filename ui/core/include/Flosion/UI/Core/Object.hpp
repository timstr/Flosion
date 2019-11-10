#pragma once

#include <GUI/GUI.hpp>

#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/NumberNode.hpp>
#include <Flosion/UI/Core/NumberPegs.hpp>
#include <Flosion/UI/Core/SoundPegs.hpp>

namespace flui {

    // TODO:
    // give peg and wire functions same names for Sound and Number
    // so that overloading can be used
    // e.g. makeInput(flo::NumberInput*), makeInput(flo::SoundInput*)

	class NumberInputPeg;
	class NumberOutputPeg;
	class NumberWire;
	class SoundInputPeg;
	class SoundOutputPeg;
	//class SoundWire;
	class Panel;

	// Object is the base class to all gui components representing sound and
	// numerical processing, and their related components
	class Object : public ui::GridContainer, public ui::Control, public ui::Draggable {
    public:
		Object();
        ~Object();

		Panel* getParentPanel();

    protected:
        // Constructs a peg for the given number/sound input/output, with a desired label
        std::unique_ptr<NumberInputPeg> makePeg(flo::NumberInput* ni, ui::String label = "Number Input");
        std::unique_ptr<NumberOutputPeg> makePeg(flo::NumberSource* no, ui::String label = "Number Output");
        std::unique_ptr<SoundInputPeg> makePeg(flo::SoundInput* si, ui::String label = "Sound Input");
        std::unique_ptr<SoundOutputPeg> makePeg(flo::SoundSource* so, ui::String label = "Sound Output");

        std::unique_ptr<ui::Element> makeSimpleBody(ui::String caption, ui::Color color);
        
        const std::vector<NumberInputPeg*>& getNumberInputPegs();
        const std::vector<NumberOutputPeg*>& getNumberOutputPegs();
        const std::vector<SoundInputPeg*>& getSoundInputPegs();
        const std::vector<SoundOutputPeg*>& getSoundOutputPegs();

    protected:
        void addToLeft(std::unique_ptr<ui::Element>);
        void addToTop(std::unique_ptr<ui::Element>);
        void addToRight(std::unique_ptr<ui::Element>);

        void addDragButton();

        void setBody(std::unique_ptr<ui::Element>);

		// void showList(const std::vector<std::pair<std::string, std::function<void()>>>& items, ui::vec2 pos);

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

		Panel* m_parentPanel;

		friend class Panel;

    private:
        ui::VerticalList& m_leftContainer;
        ui::HorizontalList& m_topContainer;
        ui::VerticalList& m_rightContainer;

    private:
		std::vector<SoundInputPeg*> m_soundInputs;
		std::vector<SoundOutputPeg*> m_soundOutputs;
		std::vector<NumberInputPeg*> m_numberInputs;
		std::vector<NumberOutputPeg*> m_numberOutputs;
        
        void addPeg(SoundInputPeg*);
        void removePeg(const SoundInputPeg*);
        void addPeg(SoundOutputPeg*);
        void removePeg(const SoundOutputPeg*);
        void addPeg(NumberInputPeg*);
        void removePeg(const NumberInputPeg*);
        void addPeg(NumberOutputPeg*);
        void removePeg(const NumberOutputPeg*);
        
        template<typename Traits>
        friend class Wire;
        template<typename Traits>
        friend class InputPeg;
        template<typename Traits>
        friend class OutputPeg;
	};

} // namespace flui