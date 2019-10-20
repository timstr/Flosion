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

} // namespace flui