#pragma once

#include <GUI/GUI.hpp>
#include <Flosion/Core/NumberSource.hpp>

namespace flui {

    class Object;
    class NumberWire;

    class NumberInputPeg : public ui::Control, public ui::Boxed<ui::FreeContainer> {
    public:
        NumberInputPeg(Object* parent, flo::NumberInput* input, ui::String label);
        ~NumberInputPeg();

        NumberWire* getAttachedWire();

        flo::NumberInput* getNumberInput();

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;

        void onMouseOver() override;

        void onMouseOut() override;

    private:
        void setAttachedWire(NumberWire*);

    private:
        Object* const m_parent;
        flo::NumberInput* const m_input;
        NumberWire* m_wireIn;
        ui::Text& m_label;

        friend class NumberWire;
    };

    class NumberOutputPeg : public ui::Control, public ui::Boxed<ui::FreeContainer> {
    public:
        NumberOutputPeg(Object* parent, flo::NumberSource* output, ui::String label);
        ~NumberOutputPeg();

        bool hasAttachedWire(const NumberWire*) const;
        const std::vector<NumberWire*>& getAttachedWires();

        flo::NumberSource* getNumberSource();

    private:
        void addAttachedWire(NumberWire*);
        void removeAttachedWire(NumberWire*);

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;

        void onMouseOver() override;

        void onMouseOut() override;
        
        Object* const m_parent;
        flo::NumberSource* const m_output;
        std::vector<NumberWire*> m_wiresOut;
        ui::Text& m_label;

        friend class NumberWire;
    };

} // namespace flui
