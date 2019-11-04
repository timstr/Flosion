#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/Objects/Ensemble.hpp>

namespace flui {

    class Ensemble : public Object {
    public:
        Ensemble();

    private:
        flo::Ensemble m_ensemble;
    };

} // namespace flui
