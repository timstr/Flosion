#include <Flosion/UI/Core/SoundWire.hpp>

#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/SoundPegs.hpp>

#include <array>

namespace flui {

    // TODO: this code is nearly identical to NumberWire.cpp. Remove this duplication.

    SoundWire::SoundWire(Box* parentBox)
        : m_parentBox(parentBox)
        , m_headPeg(nullptr)
        , m_tailPeg(nullptr)
        , m_head(add<Head>(this))
        , m_tail(add<Tail>(this)){

    }

    void SoundWire::attachHeadTo(SoundInputPeg* p){
        assert(p);
        p->attachWire(this);
        assert(m_headPeg == p);
        assert(m_headPeg->getAttachedWire() == this);
    }

    void SoundWire::detachHead(){
        if (m_headPeg){
            assert(m_headPeg->getAttachedWire() == this);
            m_headPeg->detachWire();
            assert(m_headPeg == nullptr);
        }
        assert(m_headPeg == nullptr);
    }

    void SoundWire::attachTailTo(SoundOutputPeg* p){
        assert(p);
        p->attachWire(this);
        assert(m_tailPeg == p);
        assert(m_tailPeg->hasAttachedWire(this));
    }

    void SoundWire::detachTail(){
        if (m_tailPeg){
            assert(m_tailPeg->hasAttachedWire(this));
            m_tailPeg->detachWire(this);
            assert(m_tailPeg == nullptr);
        }
        assert(m_tailPeg == nullptr);
    }

    SoundInputPeg* SoundWire::getHeadPeg(){
        return m_headPeg;
    }

    SoundOutputPeg* SoundWire::getTailPeg(){
        return m_tailPeg;
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

        // TODO
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

    bool SoundWire::Head::onLeftClick(int){
        m_parentWire->detachHead();
        startDrag();
        return true;
    }

    void SoundWire::Head::onLeftRelease(){
        stopDrag();
        drop();
    }

    void SoundWire::Head::onMove(){
        m_parentWire->updatePositions();
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

    bool SoundWire::Tail::onLeftClick(int){
        m_parentWire->detachTail();
        startDrag();
        return true;
    }

    void SoundWire::Tail::onLeftRelease(){
        stopDrag();
        drop();
    }

    void SoundWire::Tail::onMove(){
        m_parentWire->updatePositions();
    }

} // namespace flui
