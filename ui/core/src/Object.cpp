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
        : ui::GridContainer(3, 3)
        , m_parentPanel(nullptr)
        , m_inflowList(
            &putCell<ui::FreeContainer>(0, 1)
            .add<ui::Boxed<ui::VerticalList>>(ui::FreeContainer::Center, ui::FreeContainer::Center)
        )
        , m_outflowList(
            &putCell<ui::FreeContainer>(2, 1)
            .add<ui::Boxed<ui::VerticalList>>(ui::FreeContainer::Center, ui::FreeContainer::Center)
        )
        , m_flowDirection(FlowDirection::Right) {

        setShrink(true);
    }

    Object::~Object(){
        if (m_parentPanel){
            m_parentPanel->removeObject(this);
        }

        assert(m_numberInputs.size() == 0);
        assert(m_numberOutputs.size() == 0);
        assert(m_soundInputs.size() == 0);
        assert(m_soundOutputs.size() == 0);
    }

    Panel* Object::getParentPanel() noexcept {
        return m_parentPanel;
    }

    const Panel* Object::getParentPanel() const noexcept {
        return m_parentPanel;
    }

    Object::FlowDirection Object::getFlowDirection() const noexcept {
        return m_flowDirection;
    }

    void Object::trySetFlowDirection(FlowDirection fd){
        if (fd == m_flowDirection){
            return;
        }
        m_flowDirection = getNewFlowDirection(fd);
        updateLayout(m_flowDirection);
    }

    std::unique_ptr<NumberInputPeg> Object::makePeg(flo::NumberInput* ni, ui::String label){
        return std::make_unique<NumberInputPeg>(this, ni, label);
    }

    std::unique_ptr<NumberOutputPeg> Object::makePeg(flo::NumberSource* no, ui::String label){
        return std::make_unique<NumberOutputPeg>(this, no, label);
    }

    std::unique_ptr<SoundInputPeg> Object::makePeg(flo::SoundInput* si, ui::String label){
        return std::make_unique<SoundInputPeg>(this, si, label);
    }

    std::unique_ptr<SoundOutputPeg> Object::makePeg(flo::SoundSource* so, ui::String label){
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

    void Object::serializePegs(Serializer& s) const {
        const auto serializeAll = [&](const auto& v) {
            for (const auto& p : v) {
                s.addPeg(p);
            }
        };
        serializeAll(m_soundInputs);
        serializeAll(m_soundOutputs);
        serializeAll(m_numberInputs);
        serializeAll(m_numberOutputs);
    }

    void Object::deserializePegs(Deserializer& d) {
        const auto deserializeAll = [&](const auto& v) {
            for (auto& p : v) {
                d.addPeg(p);
            }
        };
        deserializeAll(m_soundInputs);
        deserializeAll(m_soundOutputs);
        deserializeAll(m_numberInputs);
        deserializeAll(m_numberOutputs);
    }

    Object::FlowDirection Object::getNewFlowDirection(FlowDirection desired) const {
        return desired;
    }

    void Object::addToInflow(std::unique_ptr<ui::Element> e){
        std::visit(
            [&](auto& l){
                l->push_back(std::move(e));
            },
            m_inflowList
        );
    }

    void Object::addToOutflow(std::unique_ptr<ui::Element> e){
        std::visit(
            [&](auto& l){
                l->push_back(std::move(e));
            },
            m_outflowList
        );
    }

    void Object::initialize(){

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

    bool Object::onKeyDown(ui::Key key){
        if (!(keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift))){
            return false;
        }

        if (key == ui::Key::Left){
            trySetFlowDirection(FlowDirection::Left);
            return true;
        } else if (key == ui::Key::Right){
            trySetFlowDirection(FlowDirection::Right);
            return true;
        } else if (key == ui::Key::Up){
            trySetFlowDirection(FlowDirection::Up);
            return true;
        } else if (key == ui::Key::Down){
            trySetFlowDirection(FlowDirection::Down);
            return true;
        }
        return false;
    }

    void Object::onGainFocus(){
        bringToFront();
        for (auto& ni : m_numberInputs){
            if (auto w = ni->getAttachedWire()){
                w->bringToFront();
            }
        }
        for (auto& no : m_numberOutputs){
            for (auto& w : no->getAttachedWires()){
                w->bringToFront();
            }
        }
        for (auto& si : m_soundInputs){
            if (auto w = si->getAttachedWire()){
                w->bringToFront();
            }
        }
        for (auto& so : m_soundOutputs){
            for (auto& w : so->getAttachedWires()){
                w->bringToFront();
            }
        }
    }

    void Object::updateLayout(FlowDirection fd){
        auto stealChildren = [](auto& l){
            assert(dynamic_cast<ui::VerticalList*>(&*l) || dynamic_cast<ui::HorizontalList*>(&*l));
            std::vector<std::unique_ptr<ui::Element>> ret;
            while (!l->empty()){
                ret.push_back(l->getChild(0)->orphan());
            }
            return ret;
        };
        
        auto inflowElements = std::visit(stealChildren, m_inflowList);
        auto outflowElements = std::visit(stealChildren, m_outflowList);
        
        clearCell(0, 0);
        clearCell(0, 1);
        clearCell(0, 2);

        clearCell(1, 0);
        // center cell is not cleared because it contains the object's body content
        clearCell(1, 2);

        clearCell(2, 0);
        clearCell(2, 1);
        clearCell(2, 2);
        
        auto makeVerticalList = [](std::vector<std::unique_ptr<Element>>& elems){
            auto l = std::make_unique<ui::Boxed<ui::VerticalList>>();
            for (auto& e : elems){
                l->push_back(std::move(e));
            }
            return l;
        };

        auto makeHorizontalList = [](std::vector<std::unique_ptr<Element>>& elems){
            auto l = std::make_unique<ui::Boxed<ui::HorizontalList>>();
            for (auto& e : elems){
                l->push_back(std::move(e));
            }
            return l;
        };

        auto makeCentered = [](auto& l){
            auto fc = std::make_unique<ui::FreeContainer>();
            fc->adopt(ui::FreeContainer::Center, ui::FreeContainer::Center, std::move(l));
            return fc;
        };

        switch (fd){
        case FlowDirection::Left:
            {
                auto i = makeVerticalList(inflowElements);
                auto o = makeVerticalList(outflowElements);
                m_inflowList = i.get();
                m_outflowList = o.get();
                putCell(2, 1, makeCentered(i));
                putCell(0, 1, makeCentered(o));
                break;
            }
        case FlowDirection::Right:
            {
                auto i = makeVerticalList(inflowElements);
                auto o = makeVerticalList(outflowElements);
                m_inflowList = i.get();
                m_outflowList = o.get();
                putCell(0, 1, makeCentered(i));
                putCell(2, 1, makeCentered(o));
                break;
            }
        case FlowDirection::Up:
            {
                auto i = makeHorizontalList(inflowElements);
                auto o = makeHorizontalList(outflowElements);
                m_inflowList = i.get();
                m_outflowList = o.get();
                putCell(1, 2, makeCentered(i));
                putCell(1, 0, makeCentered(o));
                break;
            }
        case FlowDirection::Down:
            {
                auto i = makeHorizontalList(inflowElements);
                auto o = makeHorizontalList(outflowElements);
                m_inflowList = i.get();
                m_outflowList = o.get();
                putCell(1, 0, makeCentered(i));
                putCell(1, 2, makeCentered(o));
                break;
            }
        }
    }

    void Object::setBody(std::unique_ptr<Element> e){
        assert(e);
        putCell(1, 1, std::move(e));
    }

    const NumberObject* Object::toNumberObject() const noexcept {
        return nullptr;
    }

    const BorrowingNumberObject* Object::toBorrowingNumberObject() const noexcept {
        return nullptr;
    }

    const SoundObject* Object::toSoundObject() const noexcept {
        return nullptr;
    }

    NumberObject* Object::toNumberObject() noexcept {
        return const_cast<NumberObject*>(const_cast<const Object*>(this)->toNumberObject());
    }

    BorrowingNumberObject* Object::toBorrowingNumberObject() noexcept {
        return const_cast<BorrowingNumberObject*>(const_cast<const Object*>(this)->toBorrowingNumberObject());
    }

    SoundObject* Object::toSoundObject() noexcept {
        return const_cast<SoundObject*>(const_cast<const Object*>(this)->toSoundObject());
    }

    void Object::addPeg(SoundInputPeg* p){
        vectorSetAdd(m_soundInputs, p);
    }

    void Object::removePeg(const SoundInputPeg* p){
        vectorSetRemove(m_soundInputs, p);
    }

    void Object::addPeg(SoundOutputPeg* p){
        vectorSetAdd(m_soundOutputs, p);
    }

    void Object::removePeg(const SoundOutputPeg* p){
        vectorSetRemove(m_soundOutputs, p);
    }

    void Object::addPeg(NumberInputPeg* p){
        vectorSetAdd(m_numberInputs, p);
    }

    void Object::removePeg(const NumberInputPeg* p){
        vectorSetRemove(m_numberInputs, p);
    }

    void Object::addPeg(NumberOutputPeg* p){
        vectorSetAdd(m_numberOutputs, p);
    }

    void Object::removePeg(const NumberOutputPeg* p){
        vectorSetRemove(m_numberOutputs, p);
    }

} // namespace flui

