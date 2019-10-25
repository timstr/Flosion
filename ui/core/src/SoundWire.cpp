#include <Flosion/UI/Core/SoundWire.hpp>

#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/SoundPegs.hpp>

#include <array>

namespace flui {

    SoundWire::SoundWire(Box* parentBox, flo::SoundSource* src, flo::SoundInput* dst)
        : m_parentBox(parentBox)
        , m_tailPeg(m_parentBox->findPegFor(src))
        , m_headPeg(m_parentBox->findPegFor(dst))
        , m_head(add<Head>(this))
        , m_tail(add<Tail>(this)) {
    
        assert(src || dst);
        assert(!!src == !!m_tailPeg);
        assert(!!dst == !!m_headPeg);

        if (m_headPeg){
            assert(m_headPeg->getAttachedWire() == nullptr);
            m_headPeg->setAttachedWire(this);
        }

        if (m_tailPeg){
            assert(!m_tailPeg->hasAttachedWire(this));
            m_tailPeg->addAttachedWire(this);
        }

        // NOTE: It is expected that either both pegs are connected, or that
        // one peg is connected and the other peg is about to be dragged
    }

    SoundWire::~SoundWire(){
        assert(m_headPeg == nullptr);
        assert(m_tailPeg == nullptr);
    }

    void SoundWire::destroy(){
        if (m_headPeg && m_tailPeg){
            assert(m_headPeg->getAttachedWire() == this);
            assert(m_tailPeg->hasAttachedWire(this));
            assert(m_headPeg->getSoundInput());
            assert(m_tailPeg->getSoundSource());

            auto h = m_headPeg;
            auto t = m_tailPeg;

            h->setAttachedWire(nullptr);
            m_headPeg = nullptr;

            t->removeAttachedWire(this);
            m_tailPeg = nullptr;

            if (h->getSoundInput()->getSource()){
                assert(h->getSoundInput()->getSource() == t->getSoundSource());
                h->getSoundInput()->setSource(nullptr);
            }
        } else {
            if (m_headPeg){
                m_headPeg->setAttachedWire(nullptr);
                m_headPeg = nullptr;
            }
                
            if (m_tailPeg){
                m_tailPeg->removeAttachedWire(this);
                m_tailPeg = nullptr;
            }
        }
        m_parentBox->removeSoundWire(this);
    }

    SoundWire::Head* SoundWire::getHead() noexcept {
        return &m_head;
    }

    SoundWire::Tail* SoundWire::getTail() noexcept {
        return &m_tail;
    }

    SoundInputPeg* SoundWire::getHeadPeg(){
        return m_headPeg;
    }

    SoundOutputPeg* SoundWire::getTailPeg(){
        return m_tailPeg;
    }

    void SoundWire::afterInputAdded(const flo::SoundInput*){
        // Called when the attached sound source is connected to an input
        // Nothing is done here; this is handled by afterSoundSourceAdded
        // which is called simultaneously
    }

    void SoundWire::beforeInputRemoved(const flo::SoundInput*){
        // Called when the attached sound source is disconnected from its input
        // Nothing is done here; this is handled by beforeSoundSourceRemoved
        // which is called simultaneously
    }

    void SoundWire::onDestroySoundInput(){
        destroy();
    }

    void SoundWire::afterSourceAdded(const flo::SoundSource* ss){
        assert(!m_tailPeg);
        assert(m_headPeg);
        auto p = m_parentBox->findPegFor(ss);
        assert(p);
        m_tailPeg = p;
        updatePositions();
    }

    void SoundWire::beforeSourceRemoved(const flo::SoundSource* ss){
        assert(m_headPeg);
        assert(m_tailPeg);
        if (m_head.dragging()){
            m_headPeg = nullptr;
        } else if (m_tail.dragging()){
            m_tailPeg = nullptr;
        }
    }

    void SoundWire::onDestroySoundSource(){
        destroy();
    }

    void SoundWire::updatePositions(){
        const auto bp = m_parentBox->rootPos();
        
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

    void SoundWire::render(sf::RenderWindow& rw){
        assert(
            (m_headPeg && m_tailPeg) ||
            (m_headPeg && m_tail.dragging()) ||
            (m_tailPeg && m_head.dragging())
        );

        // TODO: this differs from NumberWire
        ui::FreeContainer::render(rw);
        const auto pts = std::array<sf::Vertex, 2>{
            sf::Vertex{m_tail.pos() + m_tail.size() * 0.5f, sf::Color{0x008800FF}},
            sf::Vertex{m_head.pos() + m_head.size() * 0.5f, sf::Color{0x00FF00FF}}
        };
        rw.draw(pts.data(), pts.size(), sf::PrimitiveType::Lines);
    }

    SoundWire::Head::Head(SoundWire* parentWire)
        : m_parentWire(parentWire) {

        setSize({20.0f, 20.0f}, true);
        setBackgroundColor(0xFFFFFFFF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(10.0f);
    }

    SoundWire* SoundWire::Head::getParentWire(){
        return m_parentWire;
    }

    void SoundWire::Head::disconnectAndDrag(){
        assert(m_parentWire->m_headPeg != nullptr || m_parentWire->m_tailPeg != nullptr);

        // NOTE: the head is dragged before the connection is broken so that
        // the wire does not get deleted
        setPos(m_parentWire->localMousePos() - size() * 0.5f);
        startDrag();

        if (m_parentWire->m_headPeg && m_parentWire->m_tailPeg){
            assert(m_parentWire->m_headPeg->getSoundInput());
            assert(m_parentWire->m_tailPeg->getSoundSource());
            auto i = m_parentWire->m_headPeg->getSoundInput();
            assert(i->getSource() == m_parentWire->m_tailPeg->getSoundSource());
            assert(m_parentWire->m_headPeg->getAttachedWire() == m_parentWire);
            m_parentWire->m_headPeg->setAttachedWire(nullptr);
            m_parentWire->m_headPeg = nullptr;
            i->setSource(nullptr);
        }
    }

    bool SoundWire::Head::onLeftClick(int){
        disconnectAndDrag();
        return true;
    }

    void SoundWire::Head::onLeftRelease(){
        assert(m_parentWire->m_headPeg == nullptr);
        assert(m_parentWire->m_tailPeg);
        stopDrag();
        const auto mid = size() * 0.5f;
        if (!drop(mid)){
            m_parentWire->destroy();
        }
    }

    void SoundWire::Head::onMove(){
        m_parentWire->updatePositions();
    }

    bool SoundWire::Head::onDrop(ui::Draggable* d){
        if (auto p = m_parentWire->m_headPeg){
            return p->onDrop(d);
        }
        return false;
    }

    SoundWire::Tail::Tail(SoundWire* parentWire)
        : m_parentWire(parentWire) {

        // TODO: this differs from NumberWire
        setSize({20.0f, 20.0f}, true);
        setBackgroundColor(0x40FF40FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(10.0f);
    }

    SoundWire* SoundWire::Tail::getParentWire(){
        return m_parentWire;
    }

    void SoundWire::Tail::disconnectAndDrag(){
        assert(m_parentWire->m_headPeg != nullptr || m_parentWire->m_tailPeg != nullptr);

        // NOTE: the head is dragged before the connection is broken so that
        // the wire does not get deleted
        setPos(m_parentWire->localMousePos() - size() * 0.5f);
        startDrag();

        if (m_parentWire->m_headPeg && m_parentWire->m_tailPeg){
            assert(m_parentWire->m_headPeg->getSoundInput());
            assert(m_parentWire->m_tailPeg->getSoundSource());
            auto i = m_parentWire->m_headPeg->getSoundInput();
            assert(i->getSource() == m_parentWire->m_tailPeg->getSoundSource());
            assert(m_parentWire->m_headPeg->getAttachedWire() == m_parentWire);

            m_parentWire->m_tailPeg->removeAttachedWire(m_parentWire);
            m_parentWire->m_tailPeg = nullptr;

            i->setSource(nullptr);
        }
    }

    bool SoundWire::Tail::onLeftClick(int clicks){
        if (keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift)){
            disconnectAndDrag();
            return true;
        } else {
            assert(m_parentWire->m_tailPeg);
            return m_parentWire->m_tailPeg->onLeftClick(clicks);
        }
    }

    void SoundWire::Tail::onLeftRelease(){
        assert(m_parentWire->m_headPeg);
        assert(m_parentWire->m_tailPeg == nullptr);
        stopDrag();
        const auto mid = size() * 0.5f;
        if (!drop(mid)){
            m_parentWire->destroy();
        }
    }

    void SoundWire::Tail::onMove(){
        m_parentWire->updatePositions();
    }

    bool SoundWire::Tail::onDrop(ui::Draggable* d){
        if (auto p = m_parentWire->m_tailPeg){
            return p->onDrop(d);
        }
        return false;
    }

} // namespace flui
