#include <Flosion/UI/Core/Box.hpp>

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/BoxContextMenu.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    MainBox::MainBox() {
        
    }

    Box::Box(){
        setBackgroundColor(0x080820FF);
    }

    void Box::addObject(std::unique_ptr<Object> object){
        assert(object->m_parentBox == nullptr);
        object->m_parentBox = this;
        m_objects.push_back(object.get());
        adopt(std::move(object));
    }

    NumberWire* Box::addNumberWire(){
        auto& w = add<NumberWire>(this);
        m_numberwires.push_back(&w);
        return &w;
    }

    SoundWire* Box::addSoundWire(){
        auto& w = add<SoundWire>(this);
        m_soundwires.push_back(&w);
        return &w;
    }

    void Box::render(sf::RenderWindow& rw){
        ui::BoxElement::render(rw);
        ui::FreeContainer::render(rw);
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

} // namespace flui
