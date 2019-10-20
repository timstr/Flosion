#include <Flosion/UI/Core/NumberWire.hpp>

#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/NumberPegs.hpp>

#include <array>

namespace flui {

	

    NumberWire::NumberWire(Box* parentBox)
        : m_parentBox(parentBox)
        , m_headPeg(nullptr)
        , m_tailPeg(nullptr)
        , m_head(add<Head>(this))
        , m_tail(add<Tail>(this)){

    }

    void NumberWire::attachHeadTo(NumberInputPeg* p){
        assert(p);
        p->attachWire(this);
        assert(m_headPeg == p);
        assert(m_headPeg->getAttachedWire() == this);
    }

    void NumberWire::detachHead(){
        if (m_headPeg){
            assert(m_headPeg->getAttachedWire() == this);
            m_headPeg->detachWire();
            assert(m_headPeg == nullptr);
        }
        assert(m_headPeg == nullptr);
    }

    void NumberWire::attachTailTo(NumberOutputPeg* p){
        assert(p);
        p->attachWire(this);
        assert(m_tailPeg == p);
        assert(m_tailPeg->hasAttachedWire(this));
    }

    void NumberWire::detachTail(){
        if (m_tailPeg){
            assert(m_tailPeg->hasAttachedWire(this));
            m_tailPeg->detachWire(this);
            assert(m_tailPeg == nullptr);
        }
        assert(m_tailPeg == nullptr);
    }

    NumberInputPeg* NumberWire::getHeadPeg(){
        return m_headPeg;
    }

    NumberOutputPeg* NumberWire::getTailPeg(){
        return m_tailPeg;
    }

    void NumberWire::updatePositions(){

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

    void NumberWire::render(sf::RenderWindow& rw){
        ui::FreeContainer::render(rw);
        const auto pts = std::array<sf::Vertex, 2>{
            sf::Vertex{m_tail.pos() + m_tail.size() * 0.5f, sf::Color{0x000088FF}},
            sf::Vertex{m_head.pos() + m_head.size() * 0.5f, sf::Color{0x0000FFFF}}
        };
        rw.draw(pts.data(), pts.size(), sf::PrimitiveType::Lines);
    }

    NumberWire::Head::Head(NumberWire* parentWire)
        : m_parentWire(parentWire) {

        setSize({20.0f, 20.0f}, true);
        setBackgroundColor(0xFFFFFFFF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
    }

    NumberWire* NumberWire::Head::getParentWire(){
        return m_parentWire;
    }

    bool NumberWire::Head::onLeftClick(int){
        m_parentWire->detachHead();
        startDrag();
        return true;
    }

    void NumberWire::Head::onLeftRelease(){
        stopDrag();
        drop();
    }

    void NumberWire::Head::onMove(){
        m_parentWire->updatePositions();
    }

    NumberWire::Tail::Tail(NumberWire* parentWire)
        : m_parentWire(parentWire) {

        setSize({20.0f, 20.0f}, true);
        setBackgroundColor(0xFF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(10.0f);
    }

    NumberWire* NumberWire::Tail::getParentWire(){
        return m_parentWire;
    }

    bool NumberWire::Tail::onLeftClick(int){
        m_parentWire->detachTail();
        startDrag();
        return true;
    }

    void NumberWire::Tail::onLeftRelease(){
        stopDrag();
        drop();
    }

    void NumberWire::Tail::onMove(){
        m_parentWire->updatePositions();
    }

} // namespace flui
