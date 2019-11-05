#pragma once

#include <Flosion/Core/NumberSource.hpp>

#include <GUI/GUI.hpp>

namespace flui {

    // TODO: this is virtually identical to SoundWire

    class Panel;
    class NumberInputPeg;
    class NumberOutputPeg;

    class NumberWire : public ui::FreeContainer, private flo::NumberInputReactor, private flo::NumberSourceReactor {
    public:
        NumberWire(Panel* parentPanel, flo::NumberSource* src, flo::NumberInput* dst);
        ~NumberWire();

        class Head;
        class Tail;

        void destroy();

        Head* getHead() noexcept;
        Tail* getTail() noexcept;

        NumberInputPeg* getHeadPeg();
        NumberOutputPeg* getTailPeg();
        
        class Head : public ui::Control, public ui::BoxElement, public ui::Draggable {
        public:
            Head(NumberWire* parentWire);

            NumberWire* getParentWire();

            void disconnectAndDrag();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void onMove() override;

            bool onDrop(ui::Draggable*) override;

            NumberWire* const m_parentWire;
        };

        class Tail : public ui::Control, public ui::BoxElement, public ui::Draggable {
        public:
            Tail(NumberWire* parentWire);

            NumberWire* getParentWire();

            void disconnectAndDrag();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void onMove() override;

            bool onDrop(ui::Draggable*) override;

            NumberWire* const m_parentWire;
        };

    private:
        void afterInputAdded(const flo::NumberInput*) override;
        void beforeInputRemoved(const flo::NumberInput*) override;
        void onDestroyNumberInput() override;

        void afterSourceAdded(const flo::NumberSource*) override;
        void beforeSourceRemoved(const flo::NumberSource*) override;
        void onDestroyNumberSource() override;

    private:

        void updatePositions();

        void render(sf::RenderWindow&) override;

        Panel* const m_parentPanel;
        NumberInputPeg* m_headPeg;
        NumberOutputPeg* m_tailPeg;
        Head& m_head;
        Tail& m_tail;

        friend class NumberInputPeg;
        friend class NumberOutputPeg;
        friend class Object;
    };

} // namespace flui
