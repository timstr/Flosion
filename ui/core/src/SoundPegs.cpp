#include <Flosion/UI/Core/SoundPegs.hpp>

#include <Flosion/UI/Core/Panel.hpp>
#include <Flosion/UI/Core/Font.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/Object.hpp>

namespace flui {

    SoundInputPeg::SoundInputPeg(Object* parent, flo::SoundInput* input, ui::String label)
        : m_parent(parent)
        , m_input(input)
        , m_wireIn(nullptr)
        , m_label(add<ui::Text>(
            label,
            getFont(),
            0xFFFFFFFF,
            15,
            ui::TextStyle::Italic
        )) {

        assert(m_parent);
        m_parent->addSoundInputPeg(this);
        
        // TODO: these styles differ from NumberInputPeg
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x80FF80FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);

        m_label.setVisible(false);
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
            auto w = m_parent->getParentPanel()->addSoundWire(nullptr, m_input);
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
            assert(!m_input->hasReactor(w));
            m_input->attachReactor(w);
            m_input->setSource(w->getTailPeg()->getSoundSource());
            assert(w->getHeadPeg() == this);
            assert(m_wireIn == w);
            return true;
        }
        return false;
    }
    
    void SoundInputPeg::onMouseOver(){
        auto xs = ui::FreeContainer::Center;
        auto ys = ui::FreeContainer::Center;

        auto p = rootPos() - m_parent->rootPos();

        float edgeDists[] = {
            std::abs(p.x),
            std::abs(p.x + width() - m_parent->width()),
            std::abs(p.y),
            std::abs(p.y + height() - m_parent->height())
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

    void SoundInputPeg::onMouseOut(){
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

    void SoundInputPeg::setAttachedWire(SoundWire* w){
        m_wireIn = w;
    }



    SoundOutputPeg::SoundOutputPeg(Object* parent, flo::SoundSource* output, ui::String label)
        : m_parent(parent)
        , m_output(output)
        , m_label(add<ui::Text>(
            label,
            getFont(),
            0xFFFFFFFF,
            15,
            ui::TextStyle::Italic
        )) {

        assert(m_parent);
        m_parent->addSoundOutputPeg(this);

        // TODO: these styles differ from NumberOutputPeg
        setSize({30.0f, 30.0f}, true);
        setBackgroundColor(0x408040FF);
        setBorderColor(0xFF);
        setBorderThickness(1.0f);
        setBorderRadius(5.0f);
        
        m_label.setVisible(false);
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
            auto w = m_parent->getParentPanel()->addSoundWire(m_output, nullptr);
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
            assert(!m_output->hasReactor(w));
            m_output->attachReactor(w);
            w->getHeadPeg()->getSoundInput()->setSource(m_output);
            assert(w->getTailPeg() == this);
            assert(hasAttachedWire(w));
            return true;
        }
        return false;
    }

    void SoundOutputPeg::onMouseOver(){
        auto xs = ui::FreeContainer::Center;
        auto ys = ui::FreeContainer::Center;

        auto p = rootPos() - m_parent->rootPos();

        float edgeDists[] = {
            std::abs(p.x),
            std::abs(p.x + width() - m_parent->width()),
            std::abs(p.y),
            std::abs(p.y + height() - m_parent->height())
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

    void SoundOutputPeg::onMouseOut(){
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
