#include <Flosion/UI/Core/NumberObject.hpp>
#include <Flosion/Objects/WaveTable.hpp>

namespace flui {

    // TODO: customize range

    class WaveTable : public NumberObject {
    public:
        WaveTable();

        class WaveEditor : public ui::Boxed<ui::Container>, public ui::Control {
        public:
            WaveEditor(WaveTable& parent);

            void redraw();

            void partialRedraw(std::size_t idx);

            bool onLeftClick(int) override;

            void onLeftRelease() override;

            void dragTo(ui::vec2 target);

            void render(sf::RenderWindow&) override;

            bool isLocked() const;

            void setLocked(bool);

        private:

            class Dragger : public ui::Draggable {
            public:
                Dragger(WaveEditor& waveEditor);

                void onDrag() override;

            private:
                WaveEditor& m_waveEditor;
            };

            WaveTable& m_parent;
            flo::Connection m_redrawConn;
            ui::vec2 m_prevDragPos;
            std::vector<sf::Vertex> m_vertexBuffer;

            Dragger* m_dragger;

            bool m_locked;
        };

    private:
        flo::WaveTable m_waveTable;

        WaveEditor* m_editor;

        const float m_minValue;
        const float m_maxValue;

        void serialize(Serializer&) const override;
        void deserialize(Deserializer&) override;
    };

} // namespace flui
