#include <Flosion/UI/Core/Object.hpp>

#include <Flosion/UI/Core/Panel.hpp>
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

        template<typename T, typename U>
        void vectorSetAdd(std::vector<T>& v, const U& x){
            assert(std::count(v.begin(), v.end(), x) == 0);
            v.push_back(x);
        }

        template<typename T, typename U>
        void vectorSetRemove(std::vector<T>& v, const U& x){
            assert(std::count(v.begin(), v.end(), x) == 1);
            auto it = std::find(v.begin(), v.end(), x);
            assert(it != v.end());
            v.erase(it);
        }
    }

    Object::Object()
        : ui::GridContainer(3, 2)
        , m_parentPanel(nullptr)
        , m_leftContainer(
            putCell<ui::FreeContainer>(0, 1)
            .add<ui::VerticalList>(ui::FreeContainer::Center, ui::FreeContainer::Center)
        )
        , m_topContainer(putCell<ui::HorizontalList>(1, 0)) 
        , m_rightContainer(
            putCell<ui::FreeContainer>(2, 1)
            .add<ui::VerticalList>(ui::FreeContainer::Center, ui::FreeContainer::Center)
        ) {
    }

    Object::~Object(){
        if (m_parentPanel){
            m_parentPanel->removeObject(this);
        }

        while (m_numberInputs.size() > 0){
            m_numberInputs.back()->close();
        }
        while (m_numberOutputs.size() > 0){
            m_numberOutputs.back()->close();
        }
        while (m_soundInputs.size() > 0){
            m_soundInputs.back()->close();
        }
        while (m_soundOutputs.size() > 0){
            m_soundOutputs.back()->close();
        }
    }

    Panel* Object::getParentPanel(){
        return m_parentPanel;
    }

    std::unique_ptr<NumberInputPeg> Object::makeNumberInput(flo::NumberInput* ni, ui::String label){
        return std::make_unique<NumberInputPeg>(this, ni, label);
    }

    std::unique_ptr<NumberOutputPeg> Object::makeNumberOutput(flo::NumberSource* no, ui::String label){
        return std::make_unique<NumberOutputPeg>(this, no, label);
    }

    std::unique_ptr<SoundInputPeg> Object::makeSoundInput(flo::SoundInput* si, ui::String label){
        return std::make_unique<SoundInputPeg>(this, si, label);
    }

    std::unique_ptr<SoundOutputPeg> Object::makeSoundOutput(flo::SoundSource* so, ui::String label){
        return std::make_unique<SoundOutputPeg>(this, so, label);
    }

    std::unique_ptr<ui::Element> Object::makeSimpleBody(ui::String caption, ui::Color color){
        auto b = std::make_unique<ui::Boxed<ui::FreeContainer>>();
        b->setBackgroundColor(color);
        b->setBorderColor(0xFFFFFFFF);
        b->setBorderThickness(2.0f);
        auto& c = b->add<ui::Text>(
            ui::FreeContainer::Center,
            ui::FreeContainer::Center,
            caption,
            getFont(),
            0xFFFFFFFF);
        // TODO: use container margin instead
        b->setMinSize(c.size() + ui::vec2{20.0f, 20.0f});
        return b;
    }

    const std::vector<NumberInputPeg*>& Object::getNumberInputPegs(){
        return m_numberInputs;
    }

    const std::vector<NumberOutputPeg*>& Object::getNumberOutputPegs(){
        return m_numberOutputs;
    }

    const std::vector<SoundInputPeg*>& Object::getSoundInputPegs(){
        return m_soundInputs;
    }

    const std::vector<SoundOutputPeg*>& Object::getSoundOutputPegs(){
        return m_soundOutputs;
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

    void Object::addToLeft(std::unique_ptr<ui::Element> e){
        m_leftContainer.push_back(std::move(e));
    }

    void Object::addToTop(std::unique_ptr<ui::Element> e){
        m_topContainer.push_back(std::move(e));
    }

    void Object::addtoRight(std::unique_ptr<ui::Element> e){
        m_rightContainer.push_back(std::move(e));
    }

    void Object::addDragButton(){
        putCell<DragButton>(0, 0, this);
    }

    void Object::addSoundInputPeg(SoundInputPeg* p){
        vectorSetAdd(m_soundInputs, p);
    }

    void Object::removeSoundInputPeg(const SoundInputPeg* p){
        vectorSetRemove(m_soundInputs, p);
    }

    void Object::addSoundOutputPeg(SoundOutputPeg* p){
        vectorSetAdd(m_soundOutputs, p);
    }

    void Object::removeSoundOutputPeg(const SoundOutputPeg* p){
        vectorSetRemove(m_soundOutputs, p);
    }

    void Object::addNumberInputPeg(NumberInputPeg* p){
        vectorSetAdd(m_numberInputs, p);
    }

    void Object::removeNumberInputPeg(const NumberInputPeg* p){
        vectorSetRemove(m_numberInputs, p);
    }

    void Object::addNumberOutputPeg(NumberOutputPeg* p){
        vectorSetAdd(m_numberOutputs, p);
    }

    void Object::removeNumberOutputPeg(const NumberOutputPeg* p){
        vectorSetRemove(m_numberOutputs, p);
    }

} // namespace flui

