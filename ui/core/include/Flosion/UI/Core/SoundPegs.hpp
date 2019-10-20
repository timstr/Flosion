#pragma once

#include <GUI/GUI.hpp>
#include <Flosion/Core/SoundSource.hpp>

namespace flui {

    class Object;
    class SoundWire;

    class SoundInputPeg : public ui::Control, public ui::BoxElement {
    public:
        SoundInputPeg(Object* parent, flo::SoundInput* input, ui::String label);
        ~SoundInputPeg();

        bool canAttachWire(SoundWire*) const;

        void attachWire(SoundWire*);
        void detachWire();

        SoundWire* getAttachedWire();

        flo::SoundInput* getSoundInput();

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;

        Object* const m_parent;
        flo::SoundInput* const m_input;
        SoundWire* m_wireIn;

        friend class SoundOutputPeg;
    };

    class SoundOutputPeg : public ui::Control, public ui::BoxElement {
    public:
        SoundOutputPeg(Object* parent, flo::SoundSource* output, ui::String label);
        ~SoundOutputPeg();

        bool canAttachWire(SoundWire*) const;

        void attachWire(SoundWire*);
        void detachWire(const SoundWire*);

        bool hasAttachedWire(const SoundWire*) const;
        const std::vector<SoundWire*>& getAttachedWires();

        flo::SoundSource* getSoundSource();

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;
        
        Object* const m_parent;
        flo::SoundSource* const m_output;
        std::vector<SoundWire*> m_wiresOut;

        friend class SoundInputPeg;
    };

} // namespace flui
