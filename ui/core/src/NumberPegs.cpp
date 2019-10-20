#include <Flosion/UI/Core/NumberPegs.hpp>

#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/Object.hpp>

namespace flui {

    NumberInputPeg::NumberInputPeg(Object* parent, flo::NumberInput* input, ui::String label)
        : m_parent(parent)
        , m_input(input)
        , m_wireIn(nullptr) {
        
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0xFFFFFFFF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
    }

    NumberInputPeg::~NumberInputPeg(){
        detachWire();
    }

    bool NumberInputPeg::canAttachWire(NumberWire* w) const {
        if (auto p = w->getTailPeg()){
            return m_input->canAddDependency(p->getNumberSource());
        }
        return true;
    }

    void NumberInputPeg::attachWire(NumberWire* w){
        assert(w);
        detachWire();
        assert(m_wireIn == nullptr);
        if (w->getHeadPeg()){
            throw std::runtime_error("Whoops! Please detach that wire's head first.");
        }
        m_wireIn = w;
        w->m_headPeg = this;
        if (auto tail = w->getTailPeg()){
            m_input->setSource(tail->m_output);
        }
        w->updatePositions();
    }

    void NumberInputPeg::detachWire(){
        if (m_wireIn){
            assert(m_wireIn->getHeadPeg() == this);
            if (auto tail = m_wireIn->getTailPeg()){
                assert(m_input->getSource() == tail->m_output);
                m_input->setSource(nullptr);
            }
            m_wireIn->m_headPeg = nullptr;
            m_wireIn = nullptr;
        }
    }

    NumberWire* NumberInputPeg::getAttachedWire(){
        return m_wireIn;
    }

    flo::NumberInput* NumberInputPeg::getNumberInput(){
        return m_input;
    }

    bool NumberInputPeg::onLeftClick(int clicks){
        if (m_wireIn){
            detachWire();
            m_wireIn->m_head.startDrag();
            transferEventResposeTo(&m_wireIn->m_head);
        } else {
            auto w = m_parent->getParentBox()->addNumberWire();
            attachWire(w);
            assert(m_wireIn = w);
            w->m_tail.setPos(localMousePos() + rootPos() - w->m_tail.getParentWire()->rootPos() - w->m_tail.size() * 0.5f);
            w->m_tail.startDrag();
            transferEventResposeTo(&w->m_tail);
        }
        return true;
    }

    bool NumberInputPeg::onDrop(ui::Draggable* d){
        if (auto wh = dynamic_cast<NumberWire::Head*>(d)){
            attachWire(wh->getParentWire());
            return true;
        }
        return false;
    }



    NumberOutputPeg::NumberOutputPeg(Object* parent, flo::NumberSource* output, ui::String label)
        : m_parent(parent)
        , m_output(output) {
        
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x808080FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
        
    }

    NumberOutputPeg::~NumberOutputPeg(){
        for (const auto& w : m_wiresOut){
            detachWire(w);
        }
    }

    bool NumberOutputPeg::canAttachWire(NumberWire* w) const {
        if (auto p = w->getHeadPeg()){
            return p->getNumberInput()->canAddDependency(m_output);
        }
        return true;
    }

    void NumberOutputPeg::attachWire(NumberWire* w){
        assert(w);
        assert(!hasAttachedWire(w));
        if (w->getTailPeg()){
            throw std::runtime_error("Whoops! Please detach that wire's tail first.");
        }
        m_wiresOut.push_back(w);
        w->m_tailPeg = this;
        if (auto head = w->getHeadPeg()){
            head->getNumberInput()->setSource(m_output);
        }
        w->updatePositions();
    }

    void NumberOutputPeg::detachWire(const NumberWire* w){
        assert(std::count(m_wiresOut.begin(), m_wiresOut.end(), w) == 1);
        auto it = std::find(m_wiresOut.begin(), m_wiresOut.end(), w);
        assert(it != m_wiresOut.end());

        assert((*it)->getTailPeg() == this);
        if (auto head = (*it)->getHeadPeg()){
            assert(head->getNumberInput()->getSource() == m_output);
            head->getNumberInput()->setSource(nullptr);
        }
        (*it)->m_tailPeg = nullptr;
        m_wiresOut.erase(it);
    }

    bool NumberOutputPeg::hasAttachedWire(const NumberWire* w) const {
        return std::find(m_wiresOut.begin(), m_wiresOut.end(), w) != m_wiresOut.end();
    }

    const std::vector<NumberWire*>& NumberOutputPeg::getAttachedWires(){
        return m_wiresOut;
    }

    flo::NumberSource* NumberOutputPeg::getNumberSource(){
        return m_output;
    }

    bool NumberOutputPeg::onLeftClick(int clicks){
        bool shift = keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift);
        if (shift && m_wiresOut.size() > 0){
            auto w = m_wiresOut.back();
            detachWire(w);
            w->m_tail.startDrag();
            transferEventResposeTo(&w->m_tail);
        } else {
            auto w = m_parent->getParentBox()->addNumberWire();
            attachWire(w);
            assert(hasAttachedWire(w));
            w->m_head.setPos(localMousePos() + rootPos() - w->m_head.getParentWire()->rootPos() - w->m_head.size() * 0.5f);
            w->m_head.startDrag();
            transferEventResposeTo(&w->m_head);
        }
        return true;
    }

    bool NumberOutputPeg::onDrop(ui::Draggable* d){
        if (auto wt = dynamic_cast<NumberWire::Tail*>(d)){
            attachWire(wt->getParentWire());
            return true;
        }
        return false;
    }

} // namespace flui
