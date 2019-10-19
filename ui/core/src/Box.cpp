#include <Flosion/UI/Core/Box.hpp>

#include <Flosion/UI/Core/Font.hpp>

namespace flui {

    MainBox::MainBox() {
        
    }

    Box::Box(){
        setBackgroundColor(0x808080FF);
        setBorderColor(0xFF);
        setBorderRadius(20.0f);
        setBorderThickness(1.0f);
    }

    void Box::render(sf::RenderWindow& rw){
        ui::BoxElement::render(rw);
        ui::FreeContainer::render(rw);
    }

    bool Box::onLeftClick(int clicks){
        auto mp = localMousePos();
        assert(mp);
        auto& txt = add<ui::Text>("Haha", getFont());
        txt.setPos(*mp - txt.size() * 0.5f);
        return true;
    }

} // namespace flui
