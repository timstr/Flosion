#include <Flosion/UI/Core/Panel.hpp>

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/PanelContextMenu.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/BorrowingNumberObject.hpp>
#include <Flosion/UI/Core/SoundObject.hpp>

#include <GUI/Context.hpp>

namespace flui {

    namespace {
        const float selectionDanceRadius = 5.0;
    }

    Panel::Panel()
        : m_selectionDragger(nullptr)
        , m_selection(nullptr) {
        setBackgroundColor(0x080820FF);
    }

    Panel::~Panel(){
        while (m_objects.size() > 0){
            removeObject(m_objects.back());
        }
    }

    void Panel::addObject(Object* object){
        // TODO: ensure that contained objects from the same network are added too
        // (i.e. attached borrowers and router outputs but NOT objects
        // in nested networks like in Feedback or Splicer)
        assert(object->m_parentPanel == nullptr);
        object->m_parentPanel = this;
        m_objects.push_back(object);
    }

    void Panel::removeObject(const Object* o){
        assert(o->m_parentPanel == this);
        assert(std::count(m_objects.begin(), m_objects.end(), o) == 1);
        auto it = std::find(m_objects.begin(), m_objects.end(), o);
        assert(it != m_objects.end());
        auto op = *it;
        op->m_parentPanel = nullptr;

        for (const auto& p : op->getNumberInputPegs()){
            if (auto w = p->getAttachedWire()){
                w->detachHead();
            }
        }
        for (const auto& p : op->getNumberOutputPegs()){
            while (p->getAttachedWires().size() > 0){
                p->getAttachedWires().back()->detachTail();
            }
        }
        
        for (const auto& p : op->getSoundInputPegs()){
            if (auto w = p->getAttachedWire()){
                w->detachHead();
            }
        }
        for (const auto& p : op->getSoundOutputPegs()){
            while (p->getAttachedWires().size() > 0){
                p->getAttachedWires().back()->detachTail();
            }
        }

        m_objects.erase(it);
    }

    NumberWire* Panel::addWire(flo::NumberSource* src, flo::NumberInput* dst){
        auto& w = add<NumberWire>(this, dst, src);
        m_numberwires.push_back(&w);
        return &w;
    }

    void Panel::removeWire(NumberWire* w){
        w->m_parentPanel = nullptr;
        w->destroy();
        assert(std::count(m_numberwires.begin(), m_numberwires.end(), w) == 1);
        auto it = std::find(m_numberwires.begin(), m_numberwires.end(), w);
        assert(it != m_numberwires.end());
        auto w2 = *it;
        m_numberwires.erase(it);
        w2->close();
    }

    SoundWire* Panel::addWire(flo::SoundSource* src, flo::SoundInput* dst){
        auto& w = add<SoundWire>(this, dst, src);
        m_soundwires.push_back(&w);
        return &w;
    }

    void Panel::removeWire(SoundWire* w){
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
        for (auto& o : m_objects){
            for (auto& p : o->getNumberInputPegs()){
                if (p->getInput() == ni){
                    return p;
                }
            }
        }
        return nullptr;
    }

    NumberOutputPeg* Panel::findPegFor(const flo::NumberSource* ns){
        for (auto& o : m_objects){
            for (auto& p : o->getNumberOutputPegs()){
                if (p->getOutput() == ns){
                    return p;
                }
            }
        }
        return nullptr;
    }

    SoundInputPeg* Panel::findPegFor(const flo::SoundInput* si){
        for (auto& o : m_objects){
            for (auto& p : o->getSoundInputPegs()){
                if (p->getInput() == si){
                    return p;
                }
            }
        }
        return nullptr;
    }

    SoundOutputPeg* Panel::findPegFor(const flo::SoundSource* ss){
        for (auto& o : m_objects){
            for (auto& p : o->getSoundOutputPegs()){
                if (p->getOutput() == ss){
                    return p;
                }
            }
        }
        return nullptr;
    }

    BorrowingNumberObject* Panel::findBorrowerFor(const flo::BorrowingNumberSource* bns){
        for (auto& o : m_objects){
            if (auto b = o->toBorrowingNumberObject()){
                if (b->getBorrower() == bns){
                    return b;
                }
            }
        }
        return nullptr;
    }

    std::vector<Object*> Panel::findObjectsInRect(ui::vec2 topLeft, ui::vec2 size){
        std::vector<Object*> objs;
        const auto sRect = sf::FloatRect{topLeft, size};
        for (auto& o : m_objects){
            if (o->getParentContainer() == this){
                const auto oRect = sf::FloatRect{o->pos(), o->size()};
                if (sRect.intersects(oRect)){
                    objs.push_back(o);
                }
            }
        }
        return objs;
    }

    void Panel::selectObjects(std::vector<Object*> objs){
        if (m_selection){
            m_selection->putObjectsBack();
            m_selection->close();
            m_selection = nullptr;
        }
        if (objs.size() > 0){
            m_selection = &add<SelectedObjects>(*this, objs);
            m_selection->grabFocus();
        }
    }

    bool Panel::onKeyDown(ui::Key key){
        bool ctrl = keyDown(ui::Key::LControl) || keyDown(ui::Key::RControl);
        if (key == ui::Key::A && ctrl){
            
            selectObjects(m_objects);
        }
        return false;
    }

    bool Panel::onLeftClick(int clicks){
        if (clicks == 1){
            const auto mp = localMousePos();
            m_selectionStart = mp;
            m_selectionDragger = &add<ui::Draggable>();
            m_selectionDragger->setPos(mp);
            m_selectionDragger->startDrag();
        } else if (clicks == 2){
            auto& cm = add<PanelContextMenu>(*this);
            cm.setPos(localMousePos() - cm.size() * 0.5f);
            cm.startTyping();
        }
        return true;
    }

    void Panel::onLeftRelease(){
        assert(!!m_selectionDragger == !!m_selectionStart);
        if (m_selectionDragger){
            const auto p1 = *m_selectionStart;
            const auto p2 = m_selectionDragger->pos();

            m_selectionDragger->stopDrag();
            m_selectionDragger->close();
            m_selectionDragger = nullptr;
            m_selectionStart.reset();

            selectObjects(findObjectsInRect(
                {std::min(p1.x, p2.x), std::min(p1.y, p2.y)},
                {std::abs(p1.x - p2.x), std::abs(p1.y - p2.y)}
            ));
        }
    }

    void Panel::render(sf::RenderWindow& rw){
        ui::Boxed<ui::FreeContainer>::render(rw);
        
        assert(!!m_selectionDragger == !!m_selectionStart);
        if (m_selectionDragger){
        ui::RoundedRectangle r;
            r.setFillColor(sf::Color(0xFFFF4420));
            r.setOutlineColor(sf::Color(0xFFFF4480));
            r.setOutlineThickness(2.0f);
            const auto p1 = *m_selectionStart;
            const auto p2 = m_selectionDragger->pos();
            r.setPosition({std::min(p1.x, p2.x), std::min(p1.y, p2.y)});
            r.setSize({std::abs(p1.x - p2.x), std::abs(p1.y - p2.y)});
            rw.draw(r);
        }
    }

    Panel::SelectedObjects::SelectedObjects(Panel& parentPanel, const std::vector<Object*>& objs)
        : m_parentPanel(parentPanel)
        , m_lastPos(pos()) {

        assert(objs.size() > 0);

        for (auto& o : objs){
            m_selectedObjects.push_back({o, o->pos()});
        }

        // Find the rectangular bounds of all selected objects
        ui::vec2 min = m_selectedObjects.front().first->pos();
        ui::vec2 max = m_selectedObjects.front().first->pos() + m_selectedObjects.front().first->size();

        for (auto it = ++m_selectedObjects.begin(); it != m_selectedObjects.end(); ++it){
            min.x = std::min(min.x, it->first->left());
            min.y = std::min(min.y, it->first->top());
            max.x = std::max(max.x, it->first->left() + it->first->width());
            max.y = std::max(max.y, it->first->top() + it->first->height());
        }

        const auto offset = min;
        const auto size = max - min;

        // Put rectangular element in front of objects to capture click events
        auto& fc = add<FrontCover>(*this);
        const auto margin = ui::vec2{selectionDanceRadius, selectionDanceRadius};
        fc.setPos(offset - margin);
        fc.setSize(size + 2.0f * margin);
    }

    void Panel::SelectedObjects::putObjectsBack(){
        for (auto& o : m_selectedObjects){
            o.first->setPos(o.second);
            o.first->updateWires();
        }
    }

    bool Panel::SelectedObjects::onLeftClick(int){
        startDrag();
        return true;
    }

    void Panel::SelectedObjects::onLeftRelease(){
        stopDrag();
    }

    bool Panel::SelectedObjects::onKeyDown(ui::Key key){
        const auto moveObjects = [&](ui::vec2 delta){
            for (auto& o : m_selectedObjects){
                o.second += delta;
            }
            setPos(pos() + delta);
        };

        if (key == ui::Key::Delete){
            for (const auto& o : m_selectedObjects){
                o.first->close();
            }
            m_parentPanel.m_selection = nullptr;
            close();
            return true;
        } else if (key == ui::Key::Escape){
            for (const auto& o : m_selectedObjects){
                o.first->setPos(o.second);
                o.first->updateWires();
            }
            m_parentPanel.m_selection = nullptr;
            close();
            return true;
        } else if (key == ui::Key::Up){
            moveObjects({0.0f, -10.0f});
            return true;
        } else if (key == ui::Key::Down){
            moveObjects({0.0f, 10.0f});
            return true;
        } else if (key == ui::Key::Left){
            moveObjects({-10.0f, 0.0f});
            return true;
        } else if (key == ui::Key::Right){
            moveObjects({10.0f, 0.0f});
            return true;
        }
        // TODO: cut/copy/paste (will require serialization)
        return false;
    }

    void Panel::SelectedObjects::onLoseFocus(){
        putObjectsBack();
        m_parentPanel.m_selection = nullptr;
        close();
    }

    void Panel::SelectedObjects::onDrag(){
        auto delta = pos() - m_lastPos;
        for (auto& o : m_selectedObjects){
            o.second += delta;
        }
        m_lastPos = pos();
    }

    void Panel::SelectedObjects::render(sf::RenderWindow& rw){
        auto t = ui::Context::get().getProgramTime().asSeconds();

        for (auto& o : m_selectedObjects){
            const auto r = t * 2.0f * 3.141592654f;
            const auto offset = selectionDanceRadius * ui::vec2{std::cos(r), std::sin(2.0f * r)};

            o.first->setPos(o.second + offset);
            o.first->updateWires();

            t += 0.78236f;
        }

        ui::FreeContainer::render(rw);
    }

    Panel::SelectedObjects::FrontCover::FrontCover(SelectedObjects& parent)
        : m_parent(parent) {

        setBorderRadius(10.0f);
        setBorderColor(0xFFFFFF40);
        setBorderThickness(2.0f);
        setBackgroundColor(0xFFFFFF20);
    }

    bool Panel::SelectedObjects::FrontCover::onLeftClick(int c){
        auto r = m_parent.onLeftClick(c);
        transferEventResposeTo(&m_parent);
        return r;
    }


    MainPanel::MainPanel() {
        
    }


} // namespace flui
