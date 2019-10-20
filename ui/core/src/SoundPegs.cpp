#include <Flosion/UI/Core/SoundPegs.hpp>

#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/Object.hpp>

namespace flui {

    // TODO: this code is nearly identical to NumberPegs.cpp. Remove this duplication.

    SoundInputPeg::SoundInputPeg(Object* parent, flo::SoundInput* input, ui::String label)
        : m_parent(parent)
        , m_input(input)
        , m_wireIn(nullptr) {
        
        // TODO: these styles differ from NumberInputPeg
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x80FF80FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
    }

    SoundInputPeg::~SoundInputPeg(){
        detachWire();
    }

    bool SoundInputPeg::canAttachWire(SoundWire* w) const {
        if (auto p = w->getTailPeg()){
            return m_input->canAddDependency(p->getSoundSource());
        }
        return true;
    }

    void SoundInputPeg::attachWire(SoundWire* w){
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

    void SoundInputPeg::detachWire(){
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

    SoundWire* SoundInputPeg::getAttachedWire(){
        return m_wireIn;
    }

    flo::SoundInput* SoundInputPeg::getSoundInput(){
        return m_input;
    }

    bool SoundInputPeg::onLeftClick(int clicks){
        if (m_wireIn){
            m_wireIn->m_head.startDrag();
            transferEventResposeTo(&m_wireIn->m_head);
            detachWire();
        } else {
            // TODO: this differs from NumberInputPeg
            auto w = m_parent->getParentBox()->addSoundWire();
            attachWire(w);
            assert(m_wireIn = w);
            w->m_tail.setPos(localMousePos() + rootPos() - w->m_tail.getParentWire()->rootPos() - w->m_tail.size() * 0.5f);
            w->m_tail.startDrag();
            transferEventResposeTo(&w->m_tail);
        }
        return true;
    }

    bool SoundInputPeg::onDrop(ui::Draggable* d){
        if (auto wh = dynamic_cast<SoundWire::Head*>(d)){
            attachWire(wh->getParentWire());
            return true;
        }
        return false;
    }



    SoundOutputPeg::SoundOutputPeg(Object* parent, flo::SoundSource* output, ui::String label)
        : m_parent(parent)
        , m_output(output) {
        
        // TODO: these styles differ from NumberOutputPeg
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x808080FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
        
    }

    SoundOutputPeg::~SoundOutputPeg(){
        while (m_wiresOut.size() > 0){
            detachWire(m_wiresOut.back());
        }
    }

    bool SoundOutputPeg::canAttachWire(SoundWire* w) const {
        if (auto p = w->getHeadPeg()){
            return p->getSoundInput()->canAddDependency(m_output);
        }
        return true;
    }

    void SoundOutputPeg::attachWire(SoundWire* w){
        assert(w);
        assert(!hasAttachedWire(w));
        if (w->getTailPeg()){
            throw std::runtime_error("Whoops! Please detach that wire's tail first.");
        }
        m_wiresOut.push_back(w);
        w->m_tailPeg = this;
        if (auto head = w->getHeadPeg()){
            head->getSoundInput()->setSource(m_output);
        }
        w->updatePositions();
    }

    void SoundOutputPeg::detachWire(const SoundWire* w){
        assert(std::count(m_wiresOut.begin(), m_wiresOut.end(), w) == 1);
        auto it = std::find(m_wiresOut.begin(), m_wiresOut.end(), w);
        assert(it != m_wiresOut.end());

        assert((*it)->getTailPeg() == this);
        if (auto head = (*it)->getHeadPeg()){
            assert(head->getSoundInput()->getSource() == m_output);
            head->getSoundInput()->setSource(nullptr);
        }
        (*it)->m_tailPeg = nullptr;
        m_wiresOut.erase(it);
    }

    bool SoundOutputPeg::hasAttachedWire(const SoundWire* w) const {
        return std::find(m_wiresOut.begin(), m_wiresOut.end(), w) != m_wiresOut.end();
    }

    const std::vector<SoundWire*>& SoundOutputPeg::getAttachedWires(){
        return m_wiresOut;
    }

    flo::SoundSource* SoundOutputPeg::getSoundSource(){
        return m_output;
    }

    bool SoundOutputPeg::onLeftClick(int clicks){
        bool shift = keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift);
        if (shift && m_wiresOut.size() > 0){
            auto w = m_wiresOut.back();
            detachWire(w);
            w->m_tail.startDrag();
            transferEventResposeTo(&w->m_tail);
        } else {
            // TODO: this differs from NumberOutputPeg
            auto w = m_parent->getParentBox()->addSoundWire();
            attachWire(w);
            assert(hasAttachedWire(w));
            w->m_head.setPos(localMousePos() + rootPos() - w->m_head.getParentWire()->rootPos() - w->m_head.size() * 0.5f);
            w->m_head.startDrag();
            transferEventResposeTo(&w->m_head);
        }
        return true;
    }

    bool SoundOutputPeg::onDrop(ui::Draggable* d){
        if (auto wt = dynamic_cast<SoundWire::Tail*>(d)){
            attachWire(wt->getParentWire());
            return true;
        }
        return false;
    }

} // namespace flui
