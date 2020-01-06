#include <Flosion/UI/Objects/RandomWalk.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    RandomWalk::RandomWalk()
        : BorrowingNumberObject(&m_randomWalk) {
    
        addToInflow(makePeg(&m_randomWalk.speed, "Speed"));
        addToInflow(makePeg(&m_randomWalk.damping, "Damping"));
        addToInflow(makePeg(&m_randomWalk.bias, "Bias"));
        addToOutflow(makePeg(&m_randomWalk, "Output"));
        setBody(makeSimpleBody("Random Walk", 0xe07e22ff));
    }

    RegisterFactoryObject(RandomWalk, "RandomWalk");

} // namespace flui
