#pragma once

#include <SoundNode.hpp>

namespace flo {

    class SoundSource;

    class SoundInput : public virtual SoundNode {
    public:
        SoundInput(SoundNetwork*);

        void setSource(SoundSource*) noexcept;

        SoundSource* getSource() noexcept;
        const SoundSource* getSource() const noexcept;

        class Decorator : public SoundNode::Decorator {
        public:
            
            virtual void onSetSource(const SoundSource*);

            SoundInput& self() noexcept;
            const SoundInput& self() const noexcept;
        };

    private:
        SoundSource* m_source;
    };

} // namespace flo
