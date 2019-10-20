#pragma once

#include <GUI/GUI.hpp>

namespace flui {

    class Box;
    class SoundInputPeg;
    class SoundOutputPeg;

    class SoundWire : public ui::FreeContainer {
    public:
        SoundWire(Box* parentBox);
        
        class Head : public ui::Control, public ui::BoxElement, public ui::Draggable {
        public:
            Head(SoundWire* parentWire);

            SoundWire* getParentWire();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void onMove() override;

            SoundWire* const m_parentWire;
        };

        class Tail : public ui::Control, public ui::BoxElement, public ui::Draggable {
        public:
            Tail(SoundWire* parentWire);

            SoundWire* getParentWire();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void onMove() override;

            SoundWire* const m_parentWire;
        };

        void attachHeadTo(SoundInputPeg*);
        void detachHead();

        void attachTailTo(SoundOutputPeg*);
        void detachTail();

        SoundInputPeg* getHeadPeg();
        SoundOutputPeg* getTailPeg();

    private:

        void updatePositions();

        void render(sf::RenderWindow&) override;

        Box* const m_parentBox;
        SoundInputPeg* m_headPeg;
        SoundOutputPeg* m_tailPeg;
        Head& m_head;
        Tail& m_tail;

        friend class SoundInputPeg;
        friend class SoundOutputPeg;
        friend class Object;
    };

} // namespace flui
