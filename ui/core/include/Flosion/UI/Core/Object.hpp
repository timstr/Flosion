#pragma once

#include <GUI/GUI.hpp>

#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/NumberNode.hpp>
#include <Flosion/UI/Core/NumberPegs.hpp>
#include <Flosion/UI/Core/SoundPegs.hpp>

#include <variant>

namespace flui {

	class NumberInputPeg;
	class NumberOutputPeg;
	// class NumberWire;
	class SoundInputPeg;
	class SoundOutputPeg;
	// class SoundWire;
	class Panel;

    class NumberObject;
    class BorrowingNumberObject;
    class SoundObject;

	// Object is the base class to all gui components representing sound and
	// numerical processing, and their related components
	class Object : public ui::GridContainer, public ui::Control, public ui::Draggable {
    public:
		Object();
        ~Object();

		Panel* getParentPanel();

        enum class FlowDirection : std::uint8_t {
            Left,
            Right,
            Up,
            Down
        };

        // TODO
        FlowDirection getFlowDirection() const noexcept;
        void trySetFlowDirection(FlowDirection);

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

        virtual FlowDirection getNewFlowDirection(FlowDirection desired) const;

    protected:
        void addToInflow(std::unique_ptr<ui::Element>);
        void addToOutflow(std::unique_ptr<ui::Element>);
        
        void setBody(std::unique_ptr<ui::Element>);

		// void showList(const std::vector<std::pair<std::string, std::function<void()>>>& items, ui::vec2 pos);

    private:
        virtual const NumberObject* toNumberObject() const noexcept;
        virtual const BorrowingNumberObject* toBorrowingNumberObject() const noexcept;
        virtual const SoundObject* toSoundObject() const noexcept;
        
        NumberObject* toNumberObject() noexcept;
        BorrowingNumberObject* toBorrowingNumberObject() noexcept;
        SoundObject* toSoundObject() noexcept;

        // Called by Factory::createObject after most derived constructor is called.
        // Used to set up base class things which depend on more derived things
        virtual void initialize();

        friend class Factory;

    private:
		bool onLeftClick(int) override;

		void onLeftRelease() override;

		void onDrag() override;

		void updateWires();

        bool onKeyDown(ui::Key) override;

        void onGainFocus() override;

        virtual void updateLayout(FlowDirection);

		// TODO: hide invalid inputs/outputs
		//void showNumberInputList(NumberWire* wire, ui::vec2 pos);
		//void showNumberOutputList(NumberWire* wire, ui::vec2 pos);
		//void showSoundInputList(SoundWire* wire, ui::vec2 pos);
		//void showSoundOutputList(SoundWire* wire, ui::vec2 pos);

		Panel* m_parentPanel;

		friend class Panel;
        friend class SoundObject;

    private:
        using ListVariant = std::variant<ui::Boxed<ui::VerticalList>*, ui::Boxed<ui::HorizontalList>*>;

        ListVariant m_inflowList;
        ListVariant m_outflowList;

        FlowDirection m_flowDirection;

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