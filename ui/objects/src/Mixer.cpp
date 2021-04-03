#include <Flosion/UI/Objects/Mixer.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

#include <cassert>

namespace flui {

    Mixer::Mixer()
        : SoundObject(&m_mixer) {
        addToOutflow(makePeg(&m_mixer));
        setBody(makeSimpleBody("Mixer", 0xa37546ff));

        m_inputAddedConnection = m_mixer.onInputAdded.connect([this](flo::SingleSoundInput* input) {
            assert(input);
            auto pp = makePeg(input);
            auto p = pp.get();
            addToInflow(std::move(pp));

            auto conn = p->onWireRemoved.connect([this, p](const SoundWire*) {
                m_mixer.removeInput(p->getInput());
            });

            assert(m_inputConnections.find(p) == end(m_inputConnections));
            m_inputConnections.try_emplace(p, std::move(conn));
        });

        m_inputRemovedConnection = m_mixer.onInputRemoved.connect([this](flo::SingleSoundInput* input) {
            assert(input);
            auto p = getParentPanel()->findPegFor(input);
            assert(p);
            assert(hasDescendent(p));
            auto it = m_inputConnections.find(p);
            assert(it != end(m_inputConnections));
            m_inputConnections.erase(it);
            p->close();
            assert(getParentPanel()->findPegFor(input) == nullptr);
        });
    }

    bool Mixer::onDrop(ui::Draggable* d){
        if (auto wh = dynamic_cast<SoundWireHead*>(d)){
            auto w = wh->getParentWire();
            auto t = w->getTailPeg();
            assert(t);
            auto o = t->getOutput();
            auto i = m_mixer.addInput();
            if (!i->canAddDependency(o)){
                m_mixer.removeInput(i);
                // TODO: show some kind of visual feedback?
                return false;
            }
            auto p = getParentPanel()->findPegFor(i);
            assert(p);
            assert(hasDescendent(p));
            w->attachHeadTo(p);
            return true;
        }
        return false;
    }

    void Mixer::serialize(Serializer& s) const {
        auto panel = getParentPanel();
        assert(panel);
        const auto& pegs = m_mixer.getInputs();
        s.u64(pegs.size());
        for (const auto& i : pegs) {
            const auto ip = panel->findPegFor(i);
            assert(ip->getParentObject() == this);
            s.addPeg(ip);
        }
        auto op = panel->findPegFor(&m_mixer);
        assert(op->getParentObject() == this);
        s.addPeg(op);
    }

    void Mixer::deserialize(Deserializer& d) {
        auto panel = getParentPanel();
        assert(panel);
        auto numPegs = d.u64();
        for (std::uint64_t i = 0; i < numPegs; ++i) {
            auto input = m_mixer.addInput();
            auto p = panel->findPegFor(input);
            assert(p);
            assert(hasDescendent(p));
            d.addPeg(p);
        }
        auto op = panel->findPegFor(&m_mixer);
        assert(op->getParentObject() == this);
        d.addPeg(op);
    }

    RegisterFactoryObject(Mixer, "Mixer");
    REGISTER_SERIALIZABLE(Mixer, "Mixer");

} // namespace flui
