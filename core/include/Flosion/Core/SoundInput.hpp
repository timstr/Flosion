#pragma once

#include <Flosion/Core/SoundNode.hpp>
#include <Flosion/Core/Reactable.hpp>

namespace flo {
    
    class SoundInput;
    class SoundSource;

    class SoundInputReactor : public Reactor<SoundInputReactor, SoundInput> {
    public:
        virtual void afterSourceAdded(const SoundSource*) = 0;
        virtual void beforeSourceRemoved(const SoundSource*) = 0;
        virtual void onDestroySoundInput() = 0;
    };

    class SoundInput : public Realtime<SoundNode>, public Reactable<SoundInput, SoundInputReactor> {
    public:

        SoundInput() noexcept;
        ~SoundInput();

        void setSource(SoundSource*);

        SoundSource* getSource() noexcept;
        const SoundSource* getSource() const noexcept;

    private:
        SoundSource* m_source;
    };

} // namespace flo
