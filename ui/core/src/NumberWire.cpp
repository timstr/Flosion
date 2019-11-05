#include <Flosion/UI/Core/NumberWire.hpp>

#include <Flosion/UI/Core/Panel.hpp>
#include <Flosion/UI/Core/NumberPegs.hpp>

#include <array>

namespace flui {

    NumberWire::NumberWire(Panel* parentPanel, flo::NumberSource* src, flo::NumberInput* dst)
        : m_parentPanel(parentPanel)
        , m_tailPeg(m_parentPanel->findPegFor(src))
        , m_headPeg(m_parentPanel->findPegFor(dst))
        , m_head(add<Head>(this))
        , m_tail(add<Tail>(this)) {
    
        assert(src || dst);
        assert(!!src == !!m_tailPeg);
        assert(!!dst == !!m_headPeg);

        if (m_headPeg){
            assert(m_headPeg->getAttachedWire() == nullptr);
            m_headPeg->setAttachedWire(this);
            m_headPeg->getNumberInput()->attachReactor(this);
        }

        if (m_tailPeg){
            assert(!m_tailPeg->hasAttachedWire(this));
            m_tailPeg->addAttachedWire(this);
            m_tailPeg->getNumberSource()->attachReactor(this);
        }

        // NOTE: It is expected that either both pegs are connected, or that
        // one peg is connected and the other peg is about to be dragged
    }

    NumberWire::~NumberWire(){
        assert(m_headPeg == nullptr);
        assert(m_tailPeg == nullptr);
    }

    void NumberWire::destroy(){
        if (m_headPeg && m_tailPeg){
            auto i = m_headPeg->getNumberInput();
            auto o = m_tailPeg->getNumberSource();
            assert(i->getSource() == o);
            m_headPeg->getNumberInput()->setSource(nullptr);
            i->detachReactor(this);
            o->detachReactor(this);
            assert(m_headPeg == nullptr);
            assert(m_tailPeg == nullptr);
        } else if (m_headPeg){
            m_headPeg->getNumberInput()->detachReactor(this);
            m_headPeg->setAttachedWire(nullptr);
            m_headPeg = nullptr;
        } else if (m_tailPeg){
            m_tailPeg->getNumberSource()->detachReactor(this);
            m_tailPeg->removeAttachedWire(this);
            m_tailPeg = nullptr;
        }
        
        m_parentPanel->removeNumberWire(this);
    }

    NumberWire::Head* NumberWire::getHead() noexcept {
        return &m_head;
    }

    NumberWire::Tail* NumberWire::getTail() noexcept {
        return &m_tail;
    }

    NumberInputPeg* NumberWire::getHeadPeg(){
        return m_headPeg;
    }

    NumberOutputPeg* NumberWire::getTailPeg(){
        return m_tailPeg;
    }

    void NumberWire::afterInputAdded(const flo::NumberInput*){
        // Called when the attached Number source is connected to an input
        // Nothing is done here; this is handled by afterNumberSourceAdded
        // which is called simultaneously
    }

    void NumberWire::beforeInputRemoved(const flo::NumberInput*){
        // Called when the attached Number source is disconnected from its input
        // Nothing is done here; this is handled by beforeNumberSourceRemoved
        // which is called simultaneously
    }

    void NumberWire::onDestroyNumberInput(){
        destroy();
    }

    void NumberWire::afterSourceAdded(const flo::NumberSource* ns){
        if (!m_headPeg){
            auto& ni = flo::NumberInputReactor::target();
            auto p = m_parentPanel->findPegFor(&ni);
            assert(p);
            assert(p->getAttachedWire() != this);
            m_headPeg = p;
            p->setAttachedWire(this);
        }
        if (!m_tailPeg){
            auto p = m_parentPanel->findPegFor(ns);
            assert(p);
            assert(!p->hasAttachedWire(this));
            m_tailPeg = p;
            p->addAttachedWire(this);
        }
        updatePositions();
    }

    void NumberWire::beforeSourceRemoved(const flo::NumberSource* ns){
        if (!m_tail.dragging()){
            assert(m_headPeg);
            assert(m_headPeg->getAttachedWire() == this);
            m_headPeg->setAttachedWire(nullptr);
            m_headPeg = nullptr;
        }
        
        if (!m_head.dragging()){
            assert(m_tailPeg);
            assert(m_tailPeg->hasAttachedWire(this));
            m_tailPeg->removeAttachedWire(this);
            m_tailPeg = nullptr;
        }
    }

    void NumberWire::onDestroyNumberSource(){
        destroy();
    }

    void NumberWire::updatePositions(){
        const auto bp = m_parentPanel->rootPos();
        
        if (m_headPeg){
            m_head.setPos(m_headPeg->rootPos() + (m_headPeg->size() - m_head.size()) * 0.5f - bp);
        }
        if (m_tailPeg){
            m_tail.setPos(m_tailPeg->rootPos() + (m_tailPeg->size() - m_tail.size()) * 0.5f - bp);
        }
        
        // NOTE: this guarantees that even if the head and tail are moved,
        // the wire's rectangular area defined by its top left corner and size
        // is exactly small enough to contain the center of both ends.

        // TODO: this doesn't work properly
        /*auto headCenter = m_head.rootPos() + m_head.size() * 0.5f - bp;
        auto tailCenter = m_tail.rootPos() + m_tail.size() * 0.5f - bp;
        
        auto topLeft = ui::vec2{
            std::min(headCenter.x, tailCenter.x),
            std::min(headCenter.y, tailCenter.y)
        };
        auto bottomRight = ui::vec2{
            std::min(headCenter.x, tailCenter.x),
            std::min(headCenter.y, tailCenter.y)
        };

        setPos(pos() + topLeft);
        m_head.setPos(m_head.pos() - topLeft);
        m_tail.setPos(m_tail.pos() - topLeft);

        setSize(bottomRight - topLeft);*/
    }

    void NumberWire::render(sf::RenderWindow& rw){
        assert(
            (m_headPeg && m_tailPeg) ||
            (m_headPeg && m_tail.dragging()) ||
            (m_tailPeg && m_head.dragging())
        );

        // TODO: this differs from NumberWire
        ui::FreeContainer::render(rw);
        const auto pts = std::array<sf::Vertex, 2>{
            sf::Vertex{m_tail.pos() + m_tail.size() * 0.5f, sf::Color{0x000088FF}},
            sf::Vertex{m_head.pos() + m_head.size() * 0.5f, sf::Color{0x00FFFFFF}}
        };
        rw.draw(pts.data(), pts.size(), sf::PrimitiveType::Lines);
    }

    NumberWire::Head::Head(NumberWire* parentWire)
        : m_parentWire(parentWire) {

        setSize({20.0f, 20.0f}, true);
        setBackgroundColor(0xFFFFFFFF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(10.0f);
    }

    NumberWire* NumberWire::Head::getParentWire(){
        return m_parentWire;
    }

    void NumberWire::Head::disconnectAndDrag(){
        assert(m_parentWire->m_headPeg != nullptr || m_parentWire->m_tailPeg != nullptr);

        // NOTE: the head is dragged before the connection is broken so that
        // the wire does not get deleted
        setPos(m_parentWire->localMousePos() - size() * 0.5f);
        startDrag();

        if (m_parentWire->m_headPeg && m_parentWire->m_tailPeg){
            assert(m_parentWire->m_headPeg->getNumberInput());
            assert(m_parentWire->m_tailPeg->getNumberSource());
            auto i = m_parentWire->m_headPeg->getNumberInput();
            auto o = m_parentWire->m_tailPeg->getNumberSource();
            assert(i->getSource() == o);
            assert(m_parentWire->m_headPeg->getAttachedWire() == m_parentWire);
            i->setSource(nullptr);
            i->detachReactor(m_parentWire);
            assert(m_parentWire->m_headPeg == nullptr);
        }
    }

    bool NumberWire::Head::onLeftClick(int){
        disconnectAndDrag();
        return true;
    }

    void NumberWire::Head::onLeftRelease(){
        assert(m_parentWire->m_headPeg == nullptr);
        assert(m_parentWire->m_tailPeg);
        stopDrag();
        const auto mid = size() * 0.5f;
        if (!drop(mid)){
            m_parentWire->destroy();
        }
    }

    void NumberWire::Head::onMove(){
        m_parentWire->updatePositions();
    }

    bool NumberWire::Head::onDrop(ui::Draggable* d){
        if (auto p = m_parentWire->m_headPeg){
            return p->onDrop(d);
        }
        return false;
    }

    NumberWire::Tail::Tail(NumberWire* parentWire)
        : m_parentWire(parentWire) {

        // TODO: this differs from NumberWire
        setSize({20.0f, 20.0f}, true);
        setBackgroundColor(0x4040FFFF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(10.0f);
    }

    NumberWire* NumberWire::Tail::getParentWire(){
        return m_parentWire;
    }

    void NumberWire::Tail::disconnectAndDrag(){
        assert(m_parentWire->m_headPeg != nullptr || m_parentWire->m_tailPeg != nullptr);

        // NOTE: the head is dragged before the connection is broken so that
        // the wire does not get deleted
        setPos(m_parentWire->localMousePos() - size() * 0.5f);
        startDrag();

        if (m_parentWire->m_headPeg && m_parentWire->m_tailPeg){
            auto i = m_parentWire->m_headPeg->getNumberInput();
            auto o = m_parentWire->m_tailPeg->getNumberSource();
            assert(i->getSource() == o);
            assert(m_parentWire->m_tailPeg->hasAttachedWire(m_parentWire));
            i->setSource(nullptr);
            o->detachReactor(m_parentWire);
            assert(m_parentWire->m_tailPeg == nullptr);
        }
    }

    bool NumberWire::Tail::onLeftClick(int clicks){
        if (keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift)){
            disconnectAndDrag();
            return true;
        } else {
            assert(m_parentWire->m_tailPeg);
            return m_parentWire->m_tailPeg->onLeftClick(clicks);
        }
    }

    void NumberWire::Tail::onLeftRelease(){
        assert(m_parentWire->m_headPeg);
        assert(m_parentWire->m_tailPeg == nullptr);
        stopDrag();
        const auto mid = size() * 0.5f;
        if (!drop(mid)){
            m_parentWire->destroy();
        }
    }

    void NumberWire::Tail::onMove(){
        m_parentWire->updatePositions();
    }

    bool NumberWire::Tail::onDrop(ui::Draggable* d){
        if (auto p = m_parentWire->m_tailPeg){
            return p->onDrop(d);
        }
        return false;
    }

} // namespace flui
