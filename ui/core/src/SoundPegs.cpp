#include <Flosion/UI/Core/SoundPegs.hpp>

#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/Object.hpp>

namespace flui {

    // TODO: Now that sound wires/pegs are working, convert this to a set of templated classes
    // whose variations are described by a traits class
    // Example:
    // struct SoundTraits {
    //     using InputType = flo::SoundInput;
    //     using OutputType = flo::SoundSource;
    //     static void connect(InputType, SourceType);
    //     static void disconnect(InputType, SourceType);
    //     // TODO: peg and wire styles
    // };
    //
    // template<typename Traits>
    // class Wire;
    //
    // template<typename Traits>
    // class InputPeg;
    //
    // template<typename Traits>
    // class OutputPeg;
    //

    SoundInputPeg::SoundInputPeg(Object* parent, flo::SoundInput* input, ui::String label)
        : m_parent(parent)
        , m_input(input)
        , m_wireIn(nullptr) {

        assert(m_parent);
        m_parent->addSoundInputPeg(this);
        
        // TODO: these styles differ from NumberInputPeg
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x80FF80FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
    }

    SoundInputPeg::~SoundInputPeg(){
        if (m_wireIn){
            m_wireIn->destroy();
        }

        assert(m_parent);
        m_parent->removeSoundInputPeg(this);
    }

    SoundWire* SoundInputPeg::getAttachedWire(){
        return m_wireIn;
    }

    flo::SoundInput* SoundInputPeg::getSoundInput(){
        return m_input;
    }

    bool SoundInputPeg::onLeftClick(int clicks){
        if (m_wireIn){
            auto h = m_wireIn->getHead();
            h->disconnectAndDrag();
            transferEventResposeTo(h);
        } else {
            auto w = m_parent->getParentBox()->addSoundWire(nullptr, m_input);
            assert(w->getHeadPeg() == this);
            assert(w->getTailPeg() == nullptr);
            w->getTail()->disconnectAndDrag();
            transferEventResposeTo(w->getTail());
        }
        return true;
    }

    bool SoundInputPeg::onDrop(ui::Draggable* d){
        if (auto wh = dynamic_cast<SoundWire::Head*>(d)){
            auto w = wh->getParentWire();
            assert(w->getTailPeg());
            assert(w->getTailPeg()->getSoundSource());
            assert(w->getHeadPeg() == nullptr);
            if (m_wireIn){
                // NOTE: this will sever the previous connection
                m_wireIn->destroy();
            }
            assert(!m_wireIn);
            m_wireIn = w;
            w->m_headPeg = this;
            w->updatePositions();
            m_input->setSource(w->getTailPeg()->getSoundSource());
            return true;
        }
        return false;
    }

    void SoundInputPeg::setAttachedWire(SoundWire* w){
        m_wireIn = w;
    }



    SoundOutputPeg::SoundOutputPeg(Object* parent, flo::SoundSource* output, ui::String label)
        : m_parent(parent)
        , m_output(output) {

        assert(m_parent);
        m_parent->addSoundOutputPeg(this);

        // TODO: these styles differ from NumberOutputPeg
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x808080FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
        
    }

    SoundOutputPeg::~SoundOutputPeg(){
        while (m_wiresOut.size() > 0){
            m_wiresOut.back()->destroy();
        }
        m_parent->removeSoundOutputPeg(this);
    }

    bool SoundOutputPeg::hasAttachedWire(const SoundWire* w) const {
        return std::find(m_wiresOut.begin(), m_wiresOut.end(), w) != m_wiresOut.end();
    }

    const std::vector<SoundWire*>& SoundOutputPeg::getAttachedWires(){
        return m_wiresOut;
    }

    flo::SoundSource* SoundOutputPeg::getSoundSource(){
        return m_output;
    }

    void SoundOutputPeg::addAttachedWire(SoundWire* w){
        assert(std::count(m_wiresOut.begin(), m_wiresOut.end(), w) == 0);
        m_wiresOut.push_back(w);
    }

    void SoundOutputPeg::removeAttachedWire(SoundWire* w){
        assert(std::count(m_wiresOut.begin(), m_wiresOut.end(), w) == 1);
        auto it = std::find(m_wiresOut.begin(), m_wiresOut.end(), w);
        assert(it != m_wiresOut.end());
        m_wiresOut.erase(it);
    }

    bool SoundOutputPeg::onLeftClick(int clicks){
        bool shift = keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift);
        if (shift && m_wiresOut.size() > 0){
            auto w = m_wiresOut.back();
            assert(w->getHeadPeg());
            assert(w->getHeadPeg()->getSoundInput());
            assert(w->getHeadPeg()->getSoundInput()->getSource() == m_output);
            w->getTail()->disconnectAndDrag();
            transferEventResposeTo(w->getTail());
        } else {
            // TODO: this differs from NumberOutputPeg
            auto w = m_parent->getParentBox()->addSoundWire(m_output, nullptr);
            assert(w->getTailPeg() == this);
            assert(w->getHeadPeg() == nullptr);
            w->getHead()->disconnectAndDrag();
            transferEventResposeTo(w->getHead());
        }
        return true;
    }

    bool SoundOutputPeg::onDrop(ui::Draggable* d){
        if (auto wt = dynamic_cast<SoundWire::Tail*>(d)){
            auto w = wt->getParentWire();
            assert(w->getHeadPeg());
            assert(w->getHeadPeg()->getSoundInput());
            assert(w->getTailPeg() == nullptr);
            assert(!hasAttachedWire(w));
            addAttachedWire(w);
            w->m_tailPeg = this;
            auto i = w->getHeadPeg()->getSoundInput();
            w->updatePositions();
            i->setSource(w->getTailPeg()->getSoundSource());
            return true;
        }
        return false;
    }

} // namespace flui
