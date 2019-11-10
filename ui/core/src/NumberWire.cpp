#include <Flosion/UI/Core/NumberWire.hpp>

#include <Flosion/UI/Core/Panel.hpp>
#include <Flosion/UI/Core/NumberPegs.hpp>

#include <array>

namespace flui {

    /*
    NumberWire::NumberWire(Panel* parentPanel, flo::NumberSource* src, flo::NumberInput* dst)
        : m_parentPanel(parentPanel)
        , m_tailPeg(nullptr)
        , m_headPeg(nullptr)
        , m_head(add<Head>(this))
        , m_tail(add<Tail>(this)) {

        if (src){
            attachTailTo(m_parentPanel->findPegFor(src));
        }

        if (dst){
            attachHeadTo(m_parentPanel->findPegFor(dst));
        }

        // NOTE: It is expected that either both pegs are connected, or that
        // one peg is connected and the other peg is about to be dragged
    }

    NumberWire::~NumberWire(){
        assert(m_headPeg == nullptr);
        assert(m_tailPeg == nullptr);
    }

    void NumberWire::destroy(){
        if (m_headPeg){
            assert(m_headPeg->getAttachedWire() == this);
            m_headPeg->setAttachedWire(nullptr);
            m_headPeg = nullptr;
        }

        if (m_tailPeg){
            assert(m_tailPeg->hasAttachedWire(this));
            m_tailPeg->removeAttachedWire(this);
            m_tailPeg = nullptr;
        }
        
        if (m_parentPanel){
            m_parentPanel->removeNumberWire(this);
        }
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

    void NumberWire::attachHeadTo(NumberInputPeg* p){
        assert(p);
        if (m_headPeg == p){
            return;
        }

        if (m_headPeg){
            assert(m_headPeg->getAttachedWire() == this);
            m_headPeg->setAttachedWire(nullptr);
        }

        m_headPeg = p;
        assert(m_headPeg->getAttachedWire() == nullptr);
        m_headPeg->setAttachedWire(this);

        auto i = p->getNumberInput();
        
        m_afterSourceAddedConn = i->afterSourceAdded.connect([&](const flo::NumberSource* src){
            attachTailTo(m_parentPanel->findPegFor(src));
        });
        m_beforeSourceRemovedConn = i->beforeSourceRemoved.connect([&](const flo::NumberSource* src){
            detachTail();
        });
        m_onDestroyNumberInputConn = i->onDestroy.connect([&](){
            detachHead();
        });

        if (m_tailPeg){
            auto o = m_tailPeg->getNumberSource();
            if (i->getSource() != o){
                i->setSource(o);
            }
        }

        updatePositions();
    }

    void NumberWire::detachHead(){
        if (m_headPeg){
            assert(m_headPeg->getAttachedWire() == this);
            m_headPeg->setAttachedWire(nullptr);
            m_headPeg = nullptr;
        }

        m_afterSourceAddedConn.reset();
        m_beforeSourceRemovedConn.reset();
        m_onDestroyNumberInputConn.reset();

        if (!m_head.dragging()){
            destroy();
        }
    }

    void NumberWire::attachTailTo(NumberOutputPeg* p){
        assert(p);

        if (m_tailPeg == p){
            return;
        }

        if (m_tailPeg){
            assert(m_tailPeg->hasAttachedWire(this));
            m_tailPeg->removeAttachedWire(this);
        }

        m_tailPeg = p;
        assert(!m_tailPeg->hasAttachedWire(this));
        m_tailPeg->addAttachedWire(this);

        auto o = m_tailPeg->getNumberSource();

        m_onDestroyNumberSourceConn = o->onDestroy.connect([&](){
            detachTail();
        });

        updatePositions();
    }

    void NumberWire::detachTail(){
        if (m_tailPeg){
            assert(m_tailPeg->hasAttachedWire(this));
            m_tailPeg->removeAttachedWire(this);
            m_tailPeg = nullptr;
        }

        m_onDestroyNumberSourceConn.reset();

        if (!m_tail.dragging()){
            destroy();
        }
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
        //auto headCenter = m_head.rootPos() + m_head.size() * 0.5f - bp;
        //auto tailCenter = m_tail.rootPos() + m_tail.size() * 0.5f - bp;
        //
        //auto topLeft = ui::vec2{
        //    std::min(headCenter.x, tailCenter.x),
        //    std::min(headCenter.y, tailCenter.y)
        //};
        //auto bottomRight = ui::vec2{
        //    std::min(headCenter.x, tailCenter.x),
        //    std::min(headCenter.y, tailCenter.y)
        //};
        //
        //setPos(pos() + topLeft);
        //m_head.setPos(m_head.pos() - topLeft);
        //m_tail.setPos(m_tail.pos() - topLeft);
        //
        //setSize(bottomRight - topLeft);
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

        m_parentWire->detachHead();
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

        m_parentWire->detachTail();
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
    */

} // namespace flui
