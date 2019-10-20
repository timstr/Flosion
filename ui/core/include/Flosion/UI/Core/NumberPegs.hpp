#pragma once

#include <GUI/GUI.hpp>
#include <Flosion/Core/NumberSource.hpp>

namespace flui {

    class Object;
    class NumberWire;

    class NumberInputPeg : public ui::Control, public ui::BoxElement {
    public:
        NumberInputPeg(Object* parent, flo::NumberInput* input, ui::String label);
        ~NumberInputPeg();

        bool canAttachWire(NumberWire*) const;

        void attachWire(NumberWire*);
        void detachWire();

        NumberWire* getAttachedWire();

        flo::NumberInput* getNumberInput();

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;

        Object* const m_parent;
        flo::NumberInput* const m_input;
        NumberWire* m_wireIn;

        friend class NumberOutputPeg;
    };

    class NumberOutputPeg : public ui::Control, public ui::BoxElement {
    public:
        NumberOutputPeg(Object* parent, flo::NumberSource* output, ui::String label);
        ~NumberOutputPeg();

        bool canAttachWire(NumberWire*) const;

        void attachWire(NumberWire*);
        void detachWire(const NumberWire*);

        bool hasAttachedWire(const NumberWire*) const;
        const std::vector<NumberWire*>& getAttachedWires();

        flo::NumberSource* getNumberSource();

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;
        
        Object* const m_parent;
        flo::NumberSource* const m_output;
        std::vector<NumberWire*> m_wiresOut;

        friend class NumberInputPeg;
    };

} // namespace flui
