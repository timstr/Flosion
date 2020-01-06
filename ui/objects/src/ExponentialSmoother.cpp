#include <Flosion/UI/Objects/ExponentialSmoother.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

    ExponentialSmoother::ExponentialSmoother()
        : BorrowingNumberObject(&m_smoother) {
    
        addToInflow(makePeg(&m_smoother.input, "Input"));
        addToInflow(makePeg(&m_smoother.decayRate, "Decay Rate"));
        addToOutflow(makePeg(&m_smoother, "Output"));

        setBody(makeSimpleBody("Exponential Smoother", 0xd9d514ff));
    }

    RegisterFactoryObject(ExponentialSmoother, "ExponentialSmoother");

} // namespace flui
