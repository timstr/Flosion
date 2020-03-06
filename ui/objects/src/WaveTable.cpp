#include <Flosion/UI/Objects/WaveTable.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>

#include <GUI/Helpers/ToggleButton.hpp>

namespace flui {

    WaveTable::WaveTable()
        : m_minValue(-1.0f)
        , m_maxValue(1.0f) {

        addToInflow(makePeg(&m_waveTable.input));
        addToOutflow(makePeg(&m_waveTable));

        auto bod = std::make_unique<ui::Boxed<ui::VerticalList>>();
        bod->setBackgroundColor(0x4a59ffff);
        bod->setBorderColor(0x000000FF);
        bod->setBorderRadius(1.0f);
        bod->setPadding(5.0f);

        m_editor = &bod->push_back<WaveEditor>(*this);

        auto& cont = bod->push_back<ui::FreeContainer>();
        cont.add<ui::ToggleButton>(
            ui::FreeContainer::Center,
            ui::FreeContainer::Center,
            m_editor->isLocked(),
            getFont(),
            [this](bool b) {
                m_editor->setLocked(b);
            },
            std::make_pair(ui::String("Lock"), ui::String("Unlock"))
        );

        setBody(std::move(bod));
    }

    void WaveTable::serialize(Serializer& s) const {
        serializePegs(s);
        auto n = m_waveTable.length();
        s << std::uint64_t{n};
        for (std::size_t i = 0; i < n; ++i) {
            s << m_waveTable.getValue(i);
        }
    }

    void WaveTable::deserialize(Deserializer& d){
        deserializePegs(d);
        auto n = std::uint64_t{};
        d >> n;
        // TODO: make the wave table resizable?
        if (n != m_waveTable.length()) {
            throw SerializationException{};
        }
        for (std::size_t i = 0; i < n; ++i) {
            auto v = double{};
            d >> v;
            m_waveTable.setValue(i, v);
        }
    }



    WaveTable::WaveEditor::WaveEditor(WaveTable& parent)
        : m_parent(parent)
        , m_prevDragPos{0.0f, 0.0f}
        , m_dragger(nullptr)
        , m_locked(true) {

        // To update styles
        setLocked(m_locked);

        setSize({256.0f, 128.0f}, true);

        redraw();

        auto& wt = m_parent.m_waveTable;
        m_redrawConn = wt.onChange.connect([this](std::size_t i) {
            partialRedraw(i);
        });
    }

    void WaveTable::WaveEditor::redraw(){
        m_vertexBuffer.clear();
        auto& wt = m_parent.m_waveTable;
        const auto n = wt.length();

        const auto getX = [&](std::size_t idx) {
            return width() * static_cast<float>(idx) / static_cast<float>(n);
        };
        const auto getY = [&](double value) {
            const auto v = std::clamp(
                static_cast<float>(value),
                m_parent.m_minValue,
                m_parent.m_maxValue
            );
            const auto t = (v - m_parent.m_minValue) / (m_parent.m_maxValue - m_parent.m_minValue);
            return (1.0f - static_cast<float>(t)) * height();
        };

        for (std::size_t i = 0; i <= n; ++i) { // NOTE: inclusive <=
            const auto x = getX(i);
            const auto y = getY(wt.getValue(i % n)); // Extra last element wraps around to 0
            m_vertexBuffer.push_back(sf::Vertex(
                sf::Vector2f{ x, y },
                sf::Color{ 0xFFFFFFFF }
            ));
        }
    }

    void WaveTable::WaveEditor::partialRedraw(std::size_t idx){
        // TODO: aaagh
        redraw();
    }

    bool WaveTable::WaveEditor::onLeftClick(int){
        if (m_locked) {
            return false;
        }

        assert(!m_dragger);

        m_dragger = &add<Dragger>(*this);
        const auto p = localMousePos();
        m_prevDragPos = p;
        m_dragger->setPos(p);
        m_dragger->startDrag();
        return true;
    }

    void WaveTable::WaveEditor::onLeftRelease(){
        assert(m_dragger);
        m_dragger->stopDrag();
        m_dragger->close();
        m_dragger = nullptr;
    }

    void WaveTable::WaveEditor::dragTo(ui::vec2 target) {
        const auto p0 = m_prevDragPos.x < target.x ? m_prevDragPos : target;
        const auto p1 = m_prevDragPos.x < target.x ? target : m_prevDragPos;

        auto& wt = m_parent.m_waveTable;
        const auto n = wt.length();

        const auto i0 = std::clamp(
            static_cast<std::size_t>(std::floor(p0.x / width() * n)),
            std::size_t{ 0 },
            n
        );
        const auto i1 = std::clamp(
            static_cast<std::size_t>(std::ceil(p1.x / width() * n)),
            std::size_t{ 0 },
            n
        );

        const auto getValue = [&](float y) {
            const auto t = 1.0f - std::clamp(y / height(), 0.0f, 1.0f);
            return t * (m_parent.m_maxValue - m_parent.m_minValue) + m_parent.m_minValue;
        };

        if (i0 == i1) {
            wt.setValue(i0 % n, getValue(target.y));
        } else {
            for (std::size_t i = i0; i <= i1; ++i) {
                const auto t = static_cast<float>(i - i0) / static_cast<float>(i1 - i0);
                const auto y = p0.y + t * (p1.y - p0.y);
                wt.setValue(i % n, getValue(y));
            }
        }
        m_prevDragPos = target;
    }

    void WaveTable::WaveEditor::render(sf::RenderWindow& rw){
        ui::BoxElement::render(rw);

        auto line = std::array<sf::Vertex, 2>{
            sf::Vertex({0.0f, height() * 0.5f}, sf::Color(0xFFFFFF80)),
            sf::Vertex({width(), height() * 0.5f}, sf::Color(0xFFFFFF80)),
        };
        rw.draw(line.data(), line.size(), sf::PrimitiveType::Lines);

        rw.draw(m_vertexBuffer.data(), m_vertexBuffer.size(), sf::PrimitiveType::LineStrip);
    }

    bool WaveTable::WaveEditor::isLocked() const {
        return m_locked;
    }

    void WaveTable::WaveEditor::setLocked(bool b){
        m_locked = b;
        if (m_locked) {
            setBackgroundColor(0x404040FF);
            setBorderColor(0x606060FF);
            setBorderThickness(0.0f);
        } else {
            setBackgroundColor(0x000000FF);
            setBorderColor(0x606060FF);
            setBorderThickness(1.0f);
        }
    }

    WaveTable::WaveEditor::Dragger::Dragger(WaveEditor& waveEditor)
        : m_waveEditor(waveEditor) {

    }

    void WaveTable::WaveEditor::Dragger::onDrag() {
        m_waveEditor.dragTo(pos());
    }

    RegisterFactoryObject(WaveTable, "WaveTable");
    REGISTER_SERIALIZABLE(WaveTable, "WaveTable");

} // namespace flui
