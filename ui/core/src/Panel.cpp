#include <Flosion/UI/Core/Panel.hpp>

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/PanelContextMenu.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Panel::Panel(){
        setBackgroundColor(0x080820FF);
    }

    Panel::~Panel(){
        while (m_objects.size() > 0){
            removeObject(m_objects.back());
        }
    }

    void Panel::addObject(std::unique_ptr<Object> object){
        assert(object->m_parentPanel == nullptr);
        object->m_parentPanel = this;
        auto op = object.get();
        m_objects.push_back(op);
        adopt(std::move(object));
        
        for (const auto& p : op->getNumberInputPegs()){
            assert(m_numberInputPegs.find(p->getNumberInput()) == m_numberInputPegs.end());
            m_numberInputPegs.emplace(p->getNumberInput(), p);
        }
        for (const auto& p : op->getNumberOutputPegs()){
            assert(m_numberOutputPegs.find(p->getNumberSource()) == m_numberOutputPegs.end());
            m_numberOutputPegs.emplace(p->getNumberSource(), p);
        }

        for (const auto& p : op->getSoundInputPegs()){
            assert(m_soundInputPegs.find(p->getSoundInput()) == m_soundInputPegs.end());
            m_soundInputPegs.emplace(p->getSoundInput(), p);
        }
        for (const auto& p : op->getSoundOutputPegs()){
            assert(m_soundOutputPegs.find(p->getSoundSource()) == m_soundOutputPegs.end());
            m_soundOutputPegs.emplace(p->getSoundSource(), p);
        }
    }

    void Panel::removeObject(const Object* o){
        assert(o->m_parentPanel == this);
        assert(std::count(m_objects.begin(), m_objects.end(), o) == 1);
        auto it = std::find(m_objects.begin(), m_objects.end(), o);
        assert(it != m_objects.end());
        auto op = *it;
        op->m_parentPanel = nullptr;

        for (const auto& p : op->getNumberInputPegs()){
            if (p->getAttachedWire()){
                p->getAttachedWire()->destroy();
            }
            auto pi = m_numberInputPegs.find(p->getNumberInput());
            assert(pi != m_numberInputPegs.end());
            m_numberInputPegs.erase(pi);
        }
        for (const auto& p : op->getNumberOutputPegs()){
            while (p->getAttachedWires().size() > 0){
                p->getAttachedWires().back()->destroy();
            }
            auto pi = m_numberOutputPegs.find(p->getNumberSource());
            assert(pi != m_numberOutputPegs.end());
            m_numberOutputPegs.erase(pi);
        }
        
        for (const auto& p : op->getSoundInputPegs()){
            if (p->getAttachedWire()){
                p->getAttachedWire()->destroy();
            }
            auto pi = m_soundInputPegs.find(p->getSoundInput());
            assert(pi != m_soundInputPegs.end());
            m_soundInputPegs.erase(pi);
        }
        for (const auto& p : op->getSoundOutputPegs()){
            while (p->getAttachedWires().size() > 0){
                p->getAttachedWires().back()->destroy();
            }
            auto pi = m_soundOutputPegs.find(p->getSoundSource());
            assert(pi != m_soundOutputPegs.end());
            m_soundOutputPegs.erase(pi);
        }

        m_objects.erase(it);
    }

    NumberWire* Panel::addNumberWire(flo::NumberSource* src, flo::NumberInput* dst){
        auto& w = add<NumberWire>(this, src, dst);
        m_numberwires.push_back(&w);
        return &w;
    }

    void Panel::removeNumberWire(NumberWire* w){
        if (w->getHeadPeg() && w->getTailPeg()){
            w->destroy();
        } else {
            assert(std::count(m_numberwires.begin(), m_numberwires.end(), w) == 1);
            auto it = std::find(m_numberwires.begin(), m_numberwires.end(), w);
            assert(it != m_numberwires.end());
            auto w2 = *it;
            m_numberwires.erase(it);
            w2->close();
        }
    }

    SoundWire* Panel::addSoundWire(flo::SoundSource* src, flo::SoundInput* dst){
        auto& w = add<SoundWire>(this, src, dst);
        m_soundwires.push_back(&w);
        return &w;
    }

    void Panel::removeSoundWire(SoundWire* w){
        if (w->getHeadPeg() && w->getTailPeg()){
            w->destroy();
        } else {
            assert(std::count(m_soundwires.begin(), m_soundwires.end(), w) == 1);
            auto it = std::find(m_soundwires.begin(), m_soundwires.end(), w);
            assert(it != m_soundwires.end());
            auto w2 = *it;
            m_soundwires.erase(it);
            w2->close();
        }
    }

    NumberInputPeg* Panel::findPegFor(const flo::NumberInput* ni){
        if (auto it = m_numberInputPegs.find(ni); it != m_numberInputPegs.end()){
            return it->second;
        }
        return nullptr;
    }

    NumberOutputPeg* Panel::findPegFor(const flo::NumberSource* ns){
        if (auto it = m_numberOutputPegs.find(ns); it != m_numberOutputPegs.end()){
            return it->second;
        }
        return nullptr;
    }

    SoundInputPeg* Panel::findPegFor(const flo::SoundInput* si){
        if (auto it = m_soundInputPegs.find(si); it != m_soundInputPegs.end()){
            return it->second;
        }
        return nullptr;
    }

    SoundOutputPeg* Panel::findPegFor(const flo::SoundSource* ss){
        if (auto it = m_soundOutputPegs.find(ss); it != m_soundOutputPegs.end()){
            return it->second;
        }
        return nullptr;
    }

    bool Panel::onLeftClick(int clicks){
        if (clicks == 2){
            auto cm = std::make_unique<PanelContextMenu>(*this);
            auto& cmr = *cm;
            cm->setPos(localMousePos() - cm->size() * 0.5f);
            adopt(std::move(cm));
            cmr.startTyping();
        }
        return true;
    }

    MainPanel::MainPanel() {
        
    }

} // namespace flui
