#include <Flosion/UI/Core/SoundObject.hpp>
#include <Flosion/Objects/Splicer.hpp>

namespace flui {

    class Splicer : public SoundObject {
    public:
        Splicer();

    private:
        bool onDrop(ui::Draggable*) override;

        flo::Splicer m_splicer;
    };

} // namespace flui
