#pragma once

#include <Flosion/Core/Signal.hpp>
#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/Panel.hpp>

#include <GUI/GUI.hpp>

namespace flui {

    // TODO: test whether it is safe to connect or disconnect

    /* The following templates are designed to be inherited from
     * public by providing a traits class as an argument which
     * must include the following public members:
     *
     * - SourceType
     *      the type of output.
     *      This is assumed to derived from flo::OutputNodeBase.
     * - InputType
     *      the type of input to which an output can be connected.
     *      This is assumed to derive from flo::InputNodeBase.
     * - InputPegType
     * - OutputPegType
     * - WireType
     * - WireHeadType
     * - WireTailType
     * - connect(InputType*, SourceType*)
     *      a static function that connects an input to an output,
     *      assuming that they are safe to connect and not yet
     *      connected.
     * - disconnect(InputType*, SourceType*)
     *      a static function that disconnects an input from
     *      an output, assuming that they are already connected
     *      and safe to disconnect
     * - canConnect(const InputType*, const OutputType*)
     *      a static function that determines whether an input
     *      may be connected to an output
     * - canDisconnect(const InputType*, const OutputType*)
     *      a static function that determines whether an input
     *      may be disconnected from an output
     */


    // Forward declarations
    template<typename Traits> class InputPeg;
    template<typename Traits> class OutputPeg;
    template<typename Traits> class Wire;
    template<typename Traits> class WireHead;
    template<typename Traits> class WireTail;

    class Peg : public ui::Control, public ui::Boxed<ui::FreeContainer> {
    public:
        Peg(Object* parentObject, ui::String label);

        Object* getParentObject() noexcept;

        void showLabel();

        void fadeLabel();

    private:
        Object* const m_parentObject;
        ui::Text& m_label;
    };

    

    template<typename Traits>
    class InputPeg : public Peg {
    public:
        using InputType = typename Traits::InputType;
        using WireType = typename Traits::WireType;

        InputPeg(Object* parentObject, InputType* input, ui::String label);
        ~InputPeg();

        WireType* getAttachedWire() noexcept;

        InputType* getInput() noexcept;

        flo::Signal<const WireType*> onWireAdded;
        flo::Signal<const WireType*> onWireRemoved;

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;

        void onMouseOver() override;

        void onMouseOut() override;

    private:
        void setAttachedWire(WireType*) noexcept;

    private:
        InputType* const m_input;
        WireType* m_wireIn;
        flo::Connection m_destroyConn;

        friend Wire<Traits>;
        friend WireHead<Traits>;
    };



    template<typename Traits>
    class OutputPeg : public Peg {
    public:

        using OutputType = typename Traits::OutputType;
        using WireType = typename Traits::WireType;

        OutputPeg(Object* parentObject, OutputType* output, ui::String label);
        ~OutputPeg();

        bool hasAttachedWire(const WireType*) const noexcept;
        const std::vector<WireType*>& getAttachedWires() noexcept;

        OutputType* getOutput() noexcept;

        flo::Signal<const WireType*> onWireAdded;
        flo::Signal<const WireType*> onWireRemoved;

    private:
        void addAttachedWire(WireType*);
        void removeAttachedWire(WireType*);

    private:
        bool onLeftClick(int) override;

        bool onDrop(ui::Draggable*) override;

        void onMouseOver() override;

        void onMouseOut() override;

        OutputType* m_output;
        std::vector<WireType*> m_wiresOut;
        flo::Connection m_destroyConn;

        friend Wire<Traits>;
        friend WireTail<Traits>;
    };



    template<typename Traits>
    class Wire : public ui::FreeContainer, public ui::Control {
    public:
        using InputType = typename Traits::InputType;
        using OutputType = typename Traits::OutputType;
        using InputPegType = typename Traits::InputPegType;
        using OutputPegType = typename Traits::OutputPegType;
        using HeadType = typename Traits::WireHeadType;
        using TailType = typename Traits::WireTailType;

        Wire(Panel* parentPanel, InputType* input, OutputType* output);
        ~Wire();

        void destroy();

        HeadType* getHead() noexcept;
        TailType* getTail() noexcept;

        InputPegType* getHeadPeg() noexcept;
        OutputPegType* getTailPeg() noexcept;

        bool canAttachHeadTo(const InputPegType*) const;
        void attachHeadTo(InputPegType*);
        void detachHead();
        
        bool canAttachTailTo(const OutputPegType*) const;
        void attachTailTo(OutputPegType*);
        void detachTail();

    private:
        flo::Connection m_onDestroyInputConn;
        flo::Connection m_onDestroyOutputConn;

        flo::Connection m_afterOutputAddedConn;
        flo::Connection m_beforeOutputRemovedConn;

    private:

        void updatePositions();

        bool m_isUpdatingPositions;

        void render(sf::RenderWindow&) override;

        void onGainFocus() override;

        Panel* m_parentPanel;
        InputPegType* m_headPeg;
        OutputPegType* m_tailPeg;
        HeadType& m_head;
        TailType& m_tail;

        friend InputPeg<Traits>;
        friend OutputPeg<Traits>;
        friend WireHead<Traits>;
        friend WireTail<Traits>;
        friend Object;
        friend Panel;
    };

    template<typename Traits>
    class WireHead : public ui::Control, public ui::BoxElement, public ui::Draggable {
    public:
        WireHead(Wire<Traits>* parentWire);

        Wire<Traits>* getParentWire() noexcept;

        void disconnectAndDrag();

    private:
        bool onLeftClick(int) override;

        void onLeftRelease() override;

        void onDrag() override;

        bool onDrop(ui::Draggable*) override;

        Wire<Traits>* const m_parentWire;
    };
    
    template<typename Traits>
    class WireTail : public ui::Control, public ui::BoxElement, public ui::Draggable {
    public:
        WireTail(Wire<Traits>* parentWire);

        Wire<Traits>* getParentWire() noexcept;

        void disconnectAndDrag();

    private:
        bool onLeftClick(int) override;

        void onLeftRelease() override;

        void onDrag() override;

        bool onDrop(ui::Draggable*) override;

        Wire<Traits>* const m_parentWire;
    };

    // TODO: move the following template definitions into a .tpp file


    template<typename Traits>
    inline InputPeg<Traits>::InputPeg(Object* parentObject, InputType* input, ui::String label)
        : Peg(parentObject, std::move(label))
        , m_input(input)
        , m_wireIn(nullptr) {

        assert(getParentObject());
        auto self = static_cast<typename Traits::InputPegType*>(this);
        getParentObject()->addPeg(self);

        m_destroyConn = m_input->onDestroy.connect([&](){
            close();
        });
    }

    template<typename Traits>
    inline InputPeg<Traits>::~InputPeg(){
        if (m_wireIn){
            m_wireIn->detachHead();
        }

        assert(getParentObject());
        auto self = static_cast<typename Traits::InputPegType*>(this);
        getParentObject()->removePeg(self);
    }

    template<typename Traits>
    inline typename InputPeg<Traits>::WireType* InputPeg<Traits>::getAttachedWire() noexcept {
        return m_wireIn;
    }

    template<typename Traits>
    inline typename InputPeg<Traits>::InputType* InputPeg<Traits>::getInput() noexcept {
        return m_input;
    }

    template<typename Traits>
    inline bool InputPeg<Traits>::onLeftClick(int){
        if (m_wireIn){
            auto h = m_wireIn->getHead();
            h->disconnectAndDrag();
            transferEventResposeTo(h);
        } else {
            auto w = getParentObject()->getParentPanel()->addWire(nullptr, m_input);
            assert(w->getHeadPeg() == this);
            assert(w->getTailPeg() == nullptr);
            w->getTail()->disconnectAndDrag();
            transferEventResposeTo(w->getTail());
        }
        return true;
    }

    template<typename Traits>
    inline bool InputPeg<Traits>::onDrop(ui::Draggable* d){
        if (m_wireIn){
            return false;
        }
        if (auto wh = dynamic_cast<typename Traits::WireHeadType*>(d)){
            auto w = wh->getParentWire();
            auto self = static_cast<typename Traits::InputPegType*>(this);
            w->attachHeadTo(self);
            return true;
        }
        return false;
    }

    template<typename Traits>
    inline void InputPeg<Traits>::onMouseOver(){
        showLabel();
    }

    template<typename Traits>
    inline void InputPeg<Traits>::onMouseOut(){
        fadeLabel();
    }

    template<typename Traits>
    inline void InputPeg<Traits>::setAttachedWire(WireType* w) noexcept {
        m_wireIn = w;
    }

    template<typename Traits>
    inline OutputPeg<Traits>::OutputPeg(Object* parentObject, OutputType* output, ui::String label)
        : Peg(parentObject, std::move(label))
        , m_output(output) {

        assert(getParentObject());
        auto self = static_cast<typename Traits::OutputPegType*>(this);
        getParentObject()->addPeg(self);

        m_destroyConn = m_output->onDestroy.connect([&](){
            close();
        });
    }

    template<typename Traits>
    inline OutputPeg<Traits>::~OutputPeg(){
        while (m_wiresOut.size() > 0){
            m_wiresOut.back()->detachTail();
        }
        auto self = static_cast<typename Traits::OutputPegType*>(this);
        getParentObject()->removePeg(self);
    }

    template<typename Traits>
    inline bool OutputPeg<Traits>::hasAttachedWire(const WireType* w) const noexcept {
        return std::find(m_wiresOut.begin(), m_wiresOut.end(), w) != m_wiresOut.end();
    }

    template<typename Traits>
    inline const std::vector<typename OutputPeg<Traits>::WireType*>& OutputPeg<Traits>::getAttachedWires() noexcept {
        return m_wiresOut;
    }

    template<typename Traits>
    inline typename OutputPeg<Traits>::OutputType* OutputPeg<Traits>::getOutput() noexcept {
        return m_output;
    }

    template<typename Traits>
    inline void OutputPeg<Traits>::addAttachedWire(WireType* w){
        assert(std::count(m_wiresOut.begin(), m_wiresOut.end(), w) == 0);
        m_wiresOut.push_back(w);
    }

    template<typename Traits>
    inline void OutputPeg<Traits>::removeAttachedWire(WireType* w){
        assert(std::count(m_wiresOut.begin(), m_wiresOut.end(), w) == 1);
        auto it = std::find(m_wiresOut.begin(), m_wiresOut.end(), w);
        assert(it != m_wiresOut.end());
        m_wiresOut.erase(it);
    }

    template<typename Traits>
    inline bool OutputPeg<Traits>::onLeftClick(int){
        bool shift = keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift);
        if (shift && m_wiresOut.size() > 0){
            auto w = m_wiresOut.back();
            assert(w->getHeadPeg());
            assert(w->getHeadPeg()->getInput());
            assert(w->getHeadPeg()->getInput()->getSource() == m_output);
            w->getTail()->disconnectAndDrag();
            transferEventResposeTo(w->getTail());
        } else {
            auto w = getParentObject()->getParentPanel()->addWire(m_output, nullptr);
            assert(w->getTailPeg() == this);
            assert(w->getHeadPeg() == nullptr);
            w->getHead()->disconnectAndDrag();
            transferEventResposeTo(w->getHead());
        }
        return true;
    }

    template<typename Traits>
    inline bool OutputPeg<Traits>::onDrop(ui::Draggable* d){
        if (auto wt = dynamic_cast<typename Traits::WireTailType*>(d)){
            auto w = wt->getParentWire();
            auto self = static_cast<typename Traits::OutputPegType*>(this);
            w->attachTailTo(self);
            return true;
        }
        return false;
    }

    template<typename Traits>
    inline void OutputPeg<Traits>::onMouseOver(){
        showLabel();
    }

    template<typename Traits>
    inline void OutputPeg<Traits>::onMouseOut(){
        fadeLabel();
    }

    template<typename Traits>
    inline Wire<Traits>::Wire(Panel* parentPanel, InputType* input, OutputType* output)
        : m_parentPanel(parentPanel)
        , m_headPeg(nullptr)
        , m_tailPeg(nullptr)
        , m_head(add<HeadType>(static_cast<typename Traits::WireType*>(this)))
        , m_tail(add<TailType>(static_cast<typename Traits::WireType*>(this)))
        , m_isUpdatingPositions(false) {

        assert(input || output);

        if (output){
            auto p = m_parentPanel->findPegFor(output);
            assert(p);
            attachTailTo(p);
        }

        if (input){
            auto p = m_parentPanel->findPegFor(input);
            assert(p);
            attachHeadTo(p);
        }

        // NOTE: It is expected that either both pegs are connected, or that
        // one peg is connected and the other peg is about to be dragged
    }

    template<typename Traits>
    inline Wire<Traits>::~Wire(){
        assert(m_headPeg == nullptr);
        assert(m_tailPeg == nullptr);
    }

    template<typename Traits>
    inline void Wire<Traits>::destroy(){
        auto self = static_cast<typename Traits::WireType*>(this);

        if (m_headPeg){
            auto h = m_headPeg;
            assert(m_headPeg->getAttachedWire() == this);
            m_headPeg->setAttachedWire(nullptr);
            m_headPeg = nullptr;
            h->onWireRemoved.broadcast(self);
        }

        if (m_tailPeg){
            auto t = m_tailPeg;
            assert(m_tailPeg->hasAttachedWire(self));
            m_tailPeg->removeAttachedWire(self);
            m_tailPeg = nullptr;
            t->onWireRemoved.broadcast(self);
        }
        
        if (m_parentPanel){
            m_parentPanel->removeWire(self);
        }
    }

    template<typename Traits>
    inline typename Wire<Traits>::HeadType* Wire<Traits>::getHead() noexcept {
        return &m_head;
    }

    template<typename Traits>
    inline typename Wire<Traits>::TailType* Wire<Traits>::getTail() noexcept {
        return &m_tail;
    }

    template<typename Traits>
    inline typename Wire<Traits>::InputPegType* Wire<Traits>::getHeadPeg() noexcept {
        return m_headPeg;
    }

    template<typename Traits>
    inline typename Wire<Traits>::OutputPegType* Wire<Traits>::getTailPeg() noexcept {
        return m_tailPeg;
    }

    template<typename Traits>
    inline bool Wire<Traits>::canAttachHeadTo(const InputPegType* p) const {
        if (m_tailPeg){
            auto i = p->getInput();
            auto o = m_tailPeg->getOutput();
            return i->canAddDependency(o);
        }
        return true;
    }

    template<typename Traits>
    inline void Wire<Traits>::attachHeadTo(InputPegType* p){
        assert(p);

        if (m_headPeg == p){
            return;
        }

        assert(!m_headPeg);

        if (auto w = p->getAttachedWire()){
            w->detachHead();
        }

        m_headPeg = p;
        assert(m_headPeg->getAttachedWire() == nullptr);
        auto self = static_cast<typename Traits::WireType*>(this);
        m_headPeg->setAttachedWire(self);

        auto i = p->getInput();
        
        m_afterOutputAddedConn = i->onSourceAdded.connect([&](const OutputType* src){
            auto p = m_parentPanel->findPegFor(src);
            assert(p);
            attachTailTo(p);
        });
        m_beforeOutputRemovedConn = i->onSourceRemoved.connect([&](const OutputType*){
            detachTail();
        });
        m_onDestroyInputConn = i->onDestroy.connect([&](){
            destroy();
        });

        updatePositions();

        if (m_tailPeg){
            auto o = m_tailPeg->getOutput();
            if (i->getSource() != o){
                i->setSource(o);
            }
        }

        p->onWireAdded.broadcast(self);
    }

    template<typename Traits>
    inline void Wire<Traits>::detachHead(){
        auto h = m_headPeg;

        if (m_headPeg){
            assert(m_headPeg->getAttachedWire() == this);
            m_headPeg->setAttachedWire(nullptr);
            m_headPeg = nullptr;
        }

        m_afterOutputAddedConn.reset();
        m_beforeOutputRemovedConn.reset();
        m_onDestroyInputConn.reset();

        if (h && m_tailPeg){
            auto i = h->getInput();
            if (i->getSource()){
                assert(i->getSource() == m_tailPeg->getOutput());
                i->setSource(nullptr);
            }
        }

        if (h){
            auto self = static_cast<typename Traits::WireType*>(this);
            h->onWireRemoved.broadcast(self);
        }

        if (!m_head.dragging()){
            destroy();
        }
    }

    template<typename Traits>
    inline bool Wire<Traits>::canAttachTailTo(const OutputPegType* p) const {
        if (m_headPeg){
            auto i = m_headPeg->getInput();
            auto o = p->getOutput();
            return i->canAddDependency(o);
        }
        return true;
    }

    template<typename Traits>
    inline void Wire<Traits>::attachTailTo(OutputPegType* p){
        assert(p);
        
        if (m_tailPeg == p){
            return;
        }

        assert(!m_tailPeg);

        auto self = static_cast<typename Traits::WireType*>(this);

        m_tailPeg = p;
        assert(!m_tailPeg->hasAttachedWire(self));
        m_tailPeg->addAttachedWire(self);

        auto o = m_tailPeg->getOutput();

        m_onDestroyOutputConn = o->onDestroy.connect([&](){
            destroy();
        });

        updatePositions();
        
        if (m_headPeg){
            auto i = m_headPeg->getInput();
            if (i->getSource() != o){
                i->setSource(o);
            }
        }

        m_tailPeg->onWireAdded.broadcast(self);
    }

    template<typename Traits>
    inline void Wire<Traits>::detachTail(){
        auto t = m_tailPeg;
        auto self = static_cast<typename Traits::WireType*>(this);

        if (m_tailPeg){
            assert(m_tailPeg->hasAttachedWire(self));
            m_tailPeg->removeAttachedWire(self);
            m_tailPeg = nullptr;
        }
        
        m_afterOutputAddedConn.reset();
        m_beforeOutputRemovedConn.reset();

        m_onDestroyOutputConn.reset();

        if (t && m_headPeg){
            auto i = m_headPeg->getInput();
            if (i->getSource()){
                assert(i->getSource() == t->getOutput());
                i->setSource(nullptr);
            }
        }

        if (t){
            t->onWireRemoved.broadcast(self);
        }

        if (!m_tail.dragging()){
            destroy();
        }
    }

    template<typename Traits>
    inline void Wire<Traits>::updatePositions(){
        if (m_isUpdatingPositions){
            return;
        }

        m_isUpdatingPositions = true;

        const auto bp = m_parentPanel->rootPos();
        
        if (m_headPeg){
            m_head.setPos(m_headPeg->rootPos() + (m_headPeg->size() - m_head.size()) * 0.5f - bp);
        }
        if (m_tailPeg){
            m_tail.setPos(m_tailPeg->rootPos() + (m_tailPeg->size() - m_tail.size()) * 0.5f - bp);
        }
        
        m_isUpdatingPositions = false;

        // NOTE: this guarantees that even if the head and tail are moved,
        // the wire's rectangular area defined by its top left corner and size
        // is exactly small enough to contain the center of both ends.

        // TODO: this doesn't work properly
        /*auto headCenter = m_head.rootPos() + m_head.size() * 0.5f - bp;
        auto tailCenter = m_tail.rootPos() + m_tail.size() * 0.5f - bp;
        
        auto topLeft = ui::vec2{
            std::min(headCenter.x, tailCenter.x),
            std::min(headCenter.y, tailCenter.y)
        };
        auto bottomRight = ui::vec2{
            std::min(headCenter.x, tailCenter.x),
            std::min(headCenter.y, tailCenter.y)
        };

        setPos(pos() + topLeft);
        m_head.setPos(m_head.pos() - topLeft);
        m_tail.setPos(m_tail.pos() - topLeft);

        setSize(bottomRight - topLeft);*/
    }

    template<typename Traits>
    inline void Wire<Traits>::render(sf::RenderWindow& rw){
        assert(
            (m_headPeg && m_tailPeg) ||
            (m_headPeg && m_tail.dragging()) ||
            (m_tailPeg && m_head.dragging())
        );

        ui::FreeContainer::render(rw);
        const auto pts = std::array<sf::Vertex, 2>{
            sf::Vertex{m_tail.pos() + m_tail.size() * 0.5f, sf::Color{0x888888FF}},
            sf::Vertex{m_head.pos() + m_head.size() * 0.5f, sf::Color{0xFFFFFFFF}}
        };
        rw.draw(pts.data(), pts.size(), sf::PrimitiveType::Lines);
    }

    template<typename Traits>
    inline void Wire<Traits>::onGainFocus(){
        this->bringToFront();
    }

    template<typename Traits>
    inline WireHead<Traits>::WireHead(Wire<Traits>* parentWire)
        : m_parentWire(parentWire) {

        setSize({20.0f, 20.0f}, true);
        setBackgroundColor(0xFFFFFFFF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(10.0f);
    }

    template<typename Traits>
    inline Wire<Traits>* WireHead<Traits>::getParentWire() noexcept {
        return m_parentWire;
    }

    template<typename Traits>
    inline void WireHead<Traits>::disconnectAndDrag(){
        assert(m_parentWire->m_headPeg != nullptr || m_parentWire->m_tailPeg != nullptr);

        // NOTE: the the wire will delete itself when detached
        // unless it is being dragged
        setPos(m_parentWire->localMousePos() - size() * 0.5f);
        startDrag();

        m_parentWire->detachHead();
    }

    template<typename Traits>
    inline bool WireHead<Traits>::onLeftClick(int){
        disconnectAndDrag();
        return true;
    }

    template<typename Traits>
    inline void WireHead<Traits>::onLeftRelease(){
        assert(m_parentWire->m_headPeg == nullptr);
        assert(m_parentWire->m_tailPeg);
        stopDrag();
        const auto mid = size() * 0.5f;
        if (!drop(mid)){
            m_parentWire->destroy();
        }
    }

    template<typename Traits>
    inline void WireHead<Traits>::onDrag(){
        m_parentWire->updatePositions();
    }

    template<typename Traits>
    inline bool WireHead<Traits>::onDrop(ui::Draggable* d){
        if (auto p = m_parentWire->m_headPeg){
            return p->onDrop(d);
        }
        return false;
    }

    template<typename Traits>
    inline WireTail<Traits>::WireTail(Wire<Traits>* parentWire)
        : m_parentWire(parentWire) {

        setSize({20.0f, 20.0f}, true);
        setBackgroundColor(0x404040FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(10.0f);
    }

    template<typename Traits>
    inline Wire<Traits>* WireTail<Traits>::getParentWire() noexcept {
        return m_parentWire;
    }

    template<typename Traits>
    inline void WireTail<Traits>::disconnectAndDrag(){
        assert(m_parentWire->m_headPeg != nullptr || m_parentWire->m_tailPeg != nullptr);
        
        // NOTE: the the wire will delete itself when detached
        // unless it is being dragged
        setPos(m_parentWire->localMousePos() - size() * 0.5f);
        startDrag();

        m_parentWire->detachTail();
    }

    template<typename Traits>
    inline bool WireTail<Traits>::onLeftClick(int clicks){
        if (keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift)){
            disconnectAndDrag();
            return true;
        } else {
            assert(m_parentWire->m_tailPeg);
            return m_parentWire->m_tailPeg->onLeftClick(clicks);
        }
    }

    template<typename Traits>
    inline void WireTail<Traits>::onLeftRelease(){
        assert(m_parentWire->m_headPeg);
        assert(m_parentWire->m_tailPeg == nullptr);
        stopDrag();
        const auto mid = size() * 0.5f;
        if (!drop(mid)){
            m_parentWire->destroy();
        }
    }

    template<typename Traits>
    inline void WireTail<Traits>::onDrag(){
        m_parentWire->updatePositions();
    }

    template<typename Traits>
    inline bool WireTail<Traits>::onDrop(ui::Draggable* d){
        if (auto p = m_parentWire->m_tailPeg){
            return p->onDrop(d);
        }
        return false;
    }

}; // namespace flui
