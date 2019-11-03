#include <Flosion/UI/Core/Box.hpp>

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/BoxContextMenu.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Box::Box(){
        setBackgroundColor(0x080820FF);
    }

    Box::~Box(){
        while (m_objects.size() > 0){
            removeObject(m_objects.back());
        }
    }

    void Box::addObject(std::unique_ptr<Object> object){
        assert(object->m_parentBox == nullptr);
        object->m_parentBox = this;
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

    void Box::removeObject(const Object* o){
        assert(o->m_parentBox == this);
        assert(std::count(m_objects.begin(), m_objects.end(), o) == 1);
        auto it = std::find(m_objects.begin(), m_objects.end(), o);
        assert(it != m_objects.end());
        auto op = *it;
        op->m_parentBox = nullptr;

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

    NumberWire* Box::addNumberWire(flo::NumberSource* src, flo::NumberInput* dst){
        auto& w = add<NumberWire>(this, src, dst);
        m_numberwires.push_back(&w);
        return &w;
    }

    void Box::removeNumberWire(NumberWire* w){
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

    SoundWire* Box::addSoundWire(flo::SoundSource* src, flo::SoundInput* dst){
        auto& w = add<SoundWire>(this, src, dst);
        m_soundwires.push_back(&w);
        return &w;
    }

    void Box::removeSoundWire(SoundWire* w){
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

    void Box::render(sf::RenderWindow& rw){
        ui::BoxElement::render(rw);
        ui::FreeContainer::render(rw);
    }

    NumberInputPeg* Box::findPegFor(const flo::NumberInput* ni){
        if (auto it = m_numberInputPegs.find(ni); it != m_numberInputPegs.end()){
            return it->second;
        }
        return nullptr;
    }

    NumberOutputPeg* Box::findPegFor(const flo::NumberSource* ns){
        if (auto it = m_numberOutputPegs.find(ns); it != m_numberOutputPegs.end()){
            return it->second;
        }
        return nullptr;
    }

    SoundInputPeg* Box::findPegFor(const flo::SoundInput* si){
        if (auto it = m_soundInputPegs.find(si); it != m_soundInputPegs.end()){
            return it->second;
        }
        return nullptr;
    }

    SoundOutputPeg* Box::findPegFor(const flo::SoundSource* ss){
        if (auto it = m_soundOutputPegs.find(ss); it != m_soundOutputPegs.end()){
            return it->second;
        }
        return nullptr;
    }

    bool Box::onLeftClick(int clicks){
        if (clicks == 2){
            auto cm = std::make_unique<BoxContextMenu>(*this);
            auto& cmr = *cm;
            cm->setPos(localMousePos() - cm->size() * 0.5f);
            adopt(std::move(cm));
            cmr.startTyping();
        }
        return true;
    }

    MainBox::MainBox() {
        
    }

} // namespace flui
