#include <Flosion/UI/Objects/Mixer.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

    Mixer::Mixer()
        : SoundObject(&m_mixer) {
        addToOutflow(makePeg(&m_mixer));
        setBody(makeSimpleBody("Mixer", 0xa37546ff));
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
                return false;
            }
            auto pp = makePeg(i);
            auto p = pp.get();
            addToInflow(std::move(pp));
            w->attachHeadTo(p);

            auto c = p->onWireRemoved.connect([this,p](const SoundWire*){
                auto it = m_connections.find(p);
                assert(it != m_connections.end());
                m_connections.erase(it);
                p->close();
            });

            m_connections.try_emplace(p, std::move(c));

            return true;
        }
        return false;
    }

    RegisterFactoryObject(Mixer, "Mixer");

} // namespace flui
