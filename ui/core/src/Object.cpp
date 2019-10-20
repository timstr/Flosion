#include <Flosion/UI/Core/Object.hpp>

#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/NumberPegs.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundPegs.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    namespace {
        const float gridSize = 30.0f;

        class DragButton : public ui::BoxElement, public ui::Control {
        public:
            DragButton(Object* parent) : m_parent(parent) {
                setSize({gridSize, gridSize}, true);
                setBackgroundColor(0x4040B0FF);
                setBorderRadius(5.0f);
            }

            bool onLeftClick(int) override {
                m_parent->startDrag();
                return true;
            }

            void onLeftRelease() override {
                m_parent->stopDrag();
            }

        private:
            Object* m_parent;
        };
    }

    Object::Object()
        : ui::GridContainer(3, 2)
        , m_parentBox(nullptr)
        , m_leftContainer(putCell<ui::GridContainer>(0, 1, 1, 0)) 
        , m_topContainer(putCell<ui::GridContainer>(1, 0, 0, 1)) 
        , m_rightContainer(
            putCell<ui::FreeContainer>(2, 1)
            .add<ui::GridContainer>(ui::FreeContainer::InsideLeft, ui::FreeContainer::Center, 1, 0)
        ) {
        
        putCell<DragButton>(0, 0, this);

        class BoxContainer : public ui::FreeContainer, public ui::BoxElement {
        private:
            void render(sf::RenderWindow& rw) override {
                ui::BoxElement::render(rw);
                ui::FreeContainer::render(rw);
            }
        };

        auto& body = putCell<BoxContainer>(1, 1);
        body.add<ui::Text>(ui::FreeContainer::Center, ui::FreeContainer::Center, "Object", getFont());
        body.setBackgroundColor(0x8080FFFF);

    }

    Object::~Object(){
        
    }

    Box* Object::getParentBox(){
        return m_parentBox;
    }

    SoundInputPeg* Object::addSoundInput(flo::SoundInput * si, ui::String label){
        m_leftContainer.appendRow();
        auto& p = m_leftContainer.putCell<SoundInputPeg>(
            0,
            m_leftContainer.rows() - 1,
            this,
            si,
            label
        );
        m_soundInputs.push_back(&p);
        return &p;
    }

    SoundOutputPeg* Object::addSoundOutput(flo::SoundSource* so, ui::String label){
        m_rightContainer.appendRow();
        auto& p = m_rightContainer.putCell<SoundOutputPeg>(
            0,
            m_rightContainer.rows() - 1,
            this,
            so,
            label
        );
        m_soundOutputs.push_back(&p);
        return &p;
    }

    NumberInputPeg* Object::addNumberInput(flo::NumberInput* ni, ui::String label){
        m_leftContainer.appendRow();
        auto& p = m_leftContainer.putCell<NumberInputPeg>(
            0,
            m_leftContainer.rows() - 1,
            this,
            ni,
            label
        );
        m_numberInputs.push_back(&p);
        return &p;
    }

    NumberOutputPeg* Object::addNumberOutput(flo::NumberSource* no, ui::String label){
        m_rightContainer.appendRow();
        auto& p = m_rightContainer.putCell<NumberOutputPeg>(
            0,
            m_rightContainer.rows() - 1,
            this,
            no,
            label
        );
        m_numberOutputs.push_back(&p);
        return &p;
    }

    bool Object::onLeftClick(int){
        startDrag();
        return true;
    }

    void Object::onLeftRelease(){
        stopDrag();
    }

    void Object::onDrag(){
        updateWires();
    }

    void Object::updateWires(){
        for (const auto& p : m_numberInputs){
            if (auto w = p->getAttachedWire()){
                w->updatePositions();
            }
        }
        for (const auto& p : m_numberOutputs){
            for (auto w : p->getAttachedWires()){
                w->updatePositions();
            }
        }
        for (const auto& p : m_soundInputs){
            if (auto w = p->getAttachedWire()){
                w->updatePositions();
            }
        }
        for (const auto& p : m_soundOutputs){
            for (auto w : p->getAttachedWires()){
                w->updatePositions();
            }
        }
    }

    void Object::setBody(std::unique_ptr<Element> e){
        assert(e);
        putCell(1, 1, std::move(e));
    }

    ui::Element* Object::getBody(){
        return getCell(1, 1);
    }

    const ui::Element* Object::getBody() const {
        return getCell(1, 1);
    }

} // namespace flui
