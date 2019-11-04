#pragma once

#include <GUI/GUI.hpp>
#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/SoundInput.hpp>

namespace flui {

    class Object;
    class SoundWire;

    class SoundInputPeg : public ui::Control, public ui::Boxed<ui::FreeContainer> {
    public:
        SoundInputPeg(Object* parent, flo::SoundInput* input, ui::String label);
        ~SoundInputPeg();

        SoundWire* getAttachedWire();

        flo::SoundInput* getSoundInput();

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;

        void onMouseOver() override;

        void onMouseOut() override;

    private:
        void setAttachedWire(SoundWire*);

    private:
        Object* const m_parent;
        flo::SoundInput* const m_input;
        SoundWire* m_wireIn;
        ui::Text& m_label;

        friend class SoundWire;
    };

    class SoundOutputPeg : public ui::Control, public ui::Boxed<ui::FreeContainer> {
    public:
        SoundOutputPeg(Object* parent, flo::SoundSource* output, ui::String label);
        ~SoundOutputPeg();

        bool hasAttachedWire(const SoundWire*) const;
        const std::vector<SoundWire*>& getAttachedWires();

        flo::SoundSource* getSoundSource();

    private:
        void addAttachedWire(SoundWire*);
        void removeAttachedWire(SoundWire*);

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;

        void onMouseOver() override;

        void onMouseOut() override;
        
        Object* const m_parent;
        flo::SoundSource* const m_output;
        std::vector<SoundWire*> m_wiresOut;
        ui::Text& m_label;

        friend class SoundWire;
    };

} // namespace flui
