#include <Flosion/UI/Core/SoundTraits.hpp>

namespace flui {

    void SoundTraits::connect(InputType* i, OutputType* o){
        i->setSource(o);
    }

    void SoundTraits::disconnect(InputType* i, OutputType* o){
        assert(i->getSource() == o);
        i->setSource(nullptr);
    }

    bool SoundTraits::isConnected(const InputType* i, const OutputType* o){
        return i->getSource() == o;
    }

    bool SoundTraits::canConnect(const InputType* i, const OutputType* o){
        return i->canAddDependency(o);
    }

    bool SoundTraits::canDisconnect(const InputType* i, const OutputType* o){
        return i->canRemoveDependency(o);
    }

} // namespace flui
