#pragma once

#include <GUI/GUI.hpp>

namespace flui {

    class Box;
    class NumberInputPeg;
    class NumberOutputPeg;

    class NumberWire : public ui::FreeContainer {
    public:
        NumberWire(Box* parentBox);
        
        class Head : public ui::Control, public ui::BoxElement, public ui::Draggable {
        public:
            Head(NumberWire* parentWire);

            NumberWire* getParentWire();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void onMove() override;

            NumberWire* const m_parentWire;
        };

        class Tail : public ui::Control, public ui::BoxElement, public ui::Draggable {
        public:
            Tail(NumberWire* parentWire);

            NumberWire* getParentWire();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void onMove() override;

            NumberWire* const m_parentWire;
        };

        void attachHeadTo(NumberInputPeg*);
        void detachHead();

        void attachTailTo(NumberOutputPeg*);
        void detachTail();

        NumberInputPeg* getHeadPeg();
        NumberOutputPeg* getTailPeg();

    private:

        void updatePositions();

        void render(sf::RenderWindow&) override;

        Box* const m_parentBox;
        NumberInputPeg* m_headPeg;
        NumberOutputPeg* m_tailPeg;
        Head& m_head;
        Tail& m_tail;

        friend class NumberInputPeg;
        friend class NumberOutputPeg;
        friend class Object;
    };

    // TODO: rethink this whole shebang

    /*
	struct NumberInput : WireInputBase<flo::NumberSource, flo::NumberInput> {
		NumberInput(ui::Ref<Object> parent_object, flo::NumberInput& target, std::string label) :
			WireInputBase(parent_object, target),
			m_label(label) {

			parent_object->addNumberInput(thisAs<NumberInput>());

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x0000FFFF));
			setMargin(5);
		}
		~NumberInput(){
			if (auto p = parentObject()){
				p->removeNumberInput(*this);
			}
		}

		const std::string& label() const {
			return m_label;
		}

	private:

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;

		const std::string m_label;
	};

	struct NumberOutput : WireOutputBase<flo::NumberSource, flo::NumberInput> {
		NumberOutput(ui::Ref<Object> parent_object, flo::NumberSource& _target, std::string label) :
			WireOutputBase(parent_object, _target),
			m_label(label) {

			parent_object->addNumberOutput(thisAs<NumberOutput>());

			setMinSize({30, 30});
			setBorderColor(sf::Color(0xFF));
			setBorderRadius(10);
			setBorderThickness(2);
			setBackgroundColor(sf::Color(0x000080FF));
			setMargin(5);
		}
		~NumberOutput(){
			if (auto p = parentObject()){
				p->removeNumberOutput(*this);
			}	
		}

		const std::string& label() const {
			return m_label;
		}

	private:

		ui::Ref<WireType> createWire(ui::Ref<Box> box) override;

		const std::string m_label;
	};

	struct NumberWire : WireBase<flo::NumberSource, flo::NumberInput> {
		NumberWire(ui::Ref<Box> parent_box) :
			WireBase(parent_box) {
			parent_box->addNumberWire(thisAs<NumberWire>());
		}
		~NumberWire() {
			if (auto pb = parentBox()) {
				pb->removeNumberWire(*this);
			}
		}

		void onConnect(flo::NumberSource& src, flo::NumberInput& dst) override {
			dst.setSource(&src);
		}

		void onDisconnect(flo::NumberSource& src, flo::NumberInput& dst) override {
			dst.setSource(nullptr);
		}
	};
    */

} // namespace flui