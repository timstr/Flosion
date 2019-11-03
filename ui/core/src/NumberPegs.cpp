#include <Flosion/UI/Core/NumberPegs.hpp>

#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/Object.hpp>

namespace flui {

    NumberInputPeg::NumberInputPeg(Object* parent, flo::NumberInput* input, ui::String label)
        : m_parent(parent)
        , m_input(input)
        , m_wireIn(nullptr) {

        assert(m_parent);
        m_parent->addNumberInputPeg(this);
        
        // TODO: these styles differ from SoundInputPeg
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x8080FFFF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
    }

    NumberInputPeg::~NumberInputPeg(){
        if (m_wireIn){
            m_wireIn->destroy();
        }

        assert(m_parent);
        m_parent->removeNumberInputPeg(this);
    }

    NumberWire* NumberInputPeg::getAttachedWire(){
        return m_wireIn;
    }

    flo::NumberInput* NumberInputPeg::getNumberInput(){
        return m_input;
    }

    bool NumberInputPeg::onLeftClick(int clicks){
        if (m_wireIn){
            auto h = m_wireIn->getHead();
            h->disconnectAndDrag();
            transferEventResposeTo(h);
        } else {
            auto w = m_parent->getParentBox()->addNumberWire(nullptr, m_input);
            assert(w->getHeadPeg() == this);
            assert(w->getTailPeg() == nullptr);
            w->getTail()->disconnectAndDrag();
            transferEventResposeTo(w->getTail());
        }
        return true;
    }

    bool NumberInputPeg::onDrop(ui::Draggable* d){
        if (auto wh = dynamic_cast<NumberWire::Head*>(d)){
            auto w = wh->getParentWire();
            assert(w->getTailPeg());
            assert(w->getTailPeg()->getNumberSource());
            assert(w->getHeadPeg() == nullptr);
            if (m_wireIn){
                // NOTE: this will sever the previous connection
                m_wireIn->destroy();
            }
            assert(!m_wireIn);
            assert(!m_input->hasReactor(w));
            m_input->attachReactor(w);
            m_input->setSource(w->getTailPeg()->getNumberSource());
            assert(w->getHeadPeg() == this);
            assert(m_wireIn == w);
            return true;
        }
        return false;
    }

    void NumberInputPeg::setAttachedWire(NumberWire* w){
        m_wireIn = w;
    }



    NumberOutputPeg::NumberOutputPeg(Object* parent, flo::NumberSource* output, ui::String label)
        : m_parent(parent)
        , m_output(output) {

        assert(m_parent);
        m_parent->addNumberOutputPeg(this);

        // TODO: these styles differ from SoundOutputPeg
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x404080FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
        
    }

    NumberOutputPeg::~NumberOutputPeg(){
        while (m_wiresOut.size() > 0){
            m_wiresOut.back()->destroy();
        }
        m_parent->removeNumberOutputPeg(this);
    }

    bool NumberOutputPeg::hasAttachedWire(const NumberWire* w) const {
        return std::find(m_wiresOut.begin(), m_wiresOut.end(), w) != m_wiresOut.end();
    }

    const std::vector<NumberWire*>& NumberOutputPeg::getAttachedWires(){
        return m_wiresOut;
    }

    flo::NumberSource* NumberOutputPeg::getNumberSource(){
        return m_output;
    }

    void NumberOutputPeg::addAttachedWire(NumberWire* w){
        assert(std::count(m_wiresOut.begin(), m_wiresOut.end(), w) == 0);
        m_wiresOut.push_back(w);
    }

    void NumberOutputPeg::removeAttachedWire(NumberWire* w){
        assert(std::count(m_wiresOut.begin(), m_wiresOut.end(), w) == 1);
        auto it = std::find(m_wiresOut.begin(), m_wiresOut.end(), w);
        assert(it != m_wiresOut.end());
        m_wiresOut.erase(it);
    }

    bool NumberOutputPeg::onLeftClick(int clicks){
        bool shift = keyDown(ui::Key::LShift) || keyDown(ui::Key::RShift);
        if (shift && m_wiresOut.size() > 0){
            auto w = m_wiresOut.back();
            assert(w->getHeadPeg());
            assert(w->getHeadPeg()->getNumberInput());
            assert(w->getHeadPeg()->getNumberInput()->getSource() == m_output);
            w->getTail()->disconnectAndDrag();
            transferEventResposeTo(w->getTail());
        } else {
            // TODO: this differs from NumberOutputPeg
            auto w = m_parent->getParentBox()->addNumberWire(m_output, nullptr);
            assert(w->getTailPeg() == this);
            assert(w->getHeadPeg() == nullptr);
            w->getHead()->disconnectAndDrag();
            transferEventResposeTo(w->getHead());
        }
        return true;
    }

    bool NumberOutputPeg::onDrop(ui::Draggable* d){
        if (auto wt = dynamic_cast<NumberWire::Tail*>(d)){
            auto w = wt->getParentWire();
            assert(w->getHeadPeg());
            assert(w->getHeadPeg()->getNumberInput());
            assert(w->getTailPeg() == nullptr);
            assert(!hasAttachedWire(w));
            assert(!m_output->hasReactor(w));
            m_output->attachReactor(w);
            w->getHeadPeg()->getNumberInput()->setSource(m_output);
            assert(w->getTailPeg() == this);
            assert(hasAttachedWire(w));
            return true;
        }
        return false;
    }

} // namespace flui
