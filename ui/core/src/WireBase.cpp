#include <Flosion/UI/Core/WireBase.hpp>

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/Font.hpp>

namespace flui {

    Peg::Peg(Object* parentObject, ui::String label)
        : m_parentObject(parentObject)
        , m_label(add<ui::Text>(
            label,
            getFont(),
            0xFFFFFFFF,
            15,
            ui::TextStyle::Italic
        )) {

        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x808080FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
        
        m_label.setVisible(false);
    }

    Object* Peg::getParentObject() noexcept {
        return m_parentObject;
    }

    const Object* Peg::getParentObject() const noexcept {
        return m_parentObject;
    }

    void Peg::showLabel(){
        auto xs = ui::FreeContainer::Center;
        auto ys = ui::FreeContainer::Center;

        auto p = rootPos() - m_parentObject->rootPos();

        float edgeDists[] = {
            std::abs(p.x),
            std::abs(p.x + width() - m_parentObject->width()),
            std::abs(p.y),
            std::abs(p.y + height() - m_parentObject->height())
        };

        auto closest = std::min_element(std::begin(edgeDists), std::end(edgeDists));

        switch (closest - std::begin(edgeDists)){
        case 0: xs = ui::FreeContainer::OutsideLeft;   break;
        case 1: xs = ui::FreeContainer::OutsideRight;  break;
        case 2: ys = ui::FreeContainer::OutsideTop;    break;
        case 3: ys = ui::FreeContainer::OutsideBottom; break;
        }

        setElementStyle(&m_label, xs, ys);
        
        m_label.clearTransitions();
        m_label.setFillColor(0xFFFFFFFF);
        m_label.setVisible(true);
    }

    void Peg::fadeLabel(){
        m_label.startTransition(
            1.0,
            [&](double t){
                m_label.setFillColor(ui::Color(
                    1.0f,
                    1.0f,
                    1.0f,
                    static_cast<float>(1.0 - t)
                ));
            },
            [&](){
                m_label.setVisible(false);
            }
        );
    }

} // namespace flui

