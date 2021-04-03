#include <Flosion/UI/Objects/Splicer.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    Splicer::Splicer()
        : SoundObject(&m_splicer) {
    
        addToInflow(makePeg(m_splicer.getSideInput(), "Side Input"));
        addToInflow(makePeg(m_splicer.getMainInput(), "Main Input"));
        addToOutflow(makePeg(&m_splicer, "Output"));

        setBody(makeSimpleBody("Splicer", 0x35135fff));
    }

    bool Splicer::onDrop(ui::Draggable* d){
        // TODO
        return false;
    }

    // TODO
    // RegisterFactoryObject(Splicer, "Splicer");
    // REGISTER_SERIALIZABLE(Splicer, "Splicer");

} // namespace flui
