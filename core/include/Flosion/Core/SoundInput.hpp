#pragma once

#include <Flosion/Core/SoundNode.hpp>

namespace flo {

    class SoundSource;

    class SoundInput : public Realtime<SoundNode> {
    public:
        SoundInput() noexcept;

        void setSource(SoundSource*);

        SoundSource* getSource() noexcept;
        const SoundSource* getSource() const noexcept;

        // TODO: should this be called Reactor?
        /*class Decorator : public SoundNode::Decorator {
        public:
            
            virtual void onSetSource(const SoundSource*);

            SoundInput& self() noexcept;
            const SoundInput& self() const noexcept;
        };*/

    private:
        SoundSource* m_source;
    };

} // namespace flo
