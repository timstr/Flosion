#pragma once

#include <Flosion/Core/SoundInput.hpp>
#include <Flosion/Core/SoundSource.hpp>

#include <GUI/GUI.hpp>

namespace flui {

    class Panel;
    class SoundInputPeg;
    class SoundOutputPeg;

    class SoundWire : public ui::FreeContainer, private flo::SoundInputReactor, private flo::SoundSourceReactor {
    public:
        SoundWire(Panel* parentPanel, flo::SoundSource* src, flo::SoundInput* dst);
        ~SoundWire();

        class Head;
        class Tail;

        void destroy();

        Head* getHead() noexcept;
        Tail* getTail() noexcept;

        SoundInputPeg* getHeadPeg();
        SoundOutputPeg* getTailPeg();
        
        class Head : public ui::Control, public ui::BoxElement, public ui::Draggable {
        public:
            Head(SoundWire* parentWire);

            SoundWire* getParentWire();

            void disconnectAndDrag();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void onMove() override;

            bool onDrop(ui::Draggable*) override;

            SoundWire* const m_parentWire;
        };

        class Tail : public ui::Control, public ui::BoxElement, public ui::Draggable {
        public:
            Tail(SoundWire* parentWire);

            SoundWire* getParentWire();

            void disconnectAndDrag();

        private:
            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void onMove() override;

            bool onDrop(ui::Draggable*) override;

            SoundWire* const m_parentWire;
        };

    private:
        void afterInputAdded(const flo::SoundInput*) override;
        void beforeInputRemoved(const flo::SoundInput*) override;
        void onDestroySoundInput() override;

        void afterSourceAdded(const flo::SoundSource*) override;
        void beforeSourceRemoved(const flo::SoundSource*) override;
        void onDestroySoundSource() override;

    private:

        void updatePositions();

        void render(sf::RenderWindow&) override;

        Panel* const m_parentPanel;
        SoundInputPeg* m_headPeg;
        SoundOutputPeg* m_tailPeg;
        Head& m_head;
        Tail& m_tail;

        friend class SoundInputPeg;
        friend class SoundOutputPeg;
        friend class Object;
    };

} // namespace flui
