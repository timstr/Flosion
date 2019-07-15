#pragma once

#include <SoundNode.hpp>

namespace flo {

    class SoundSource;

    class SoundInput : public IOSoundNode {
    public:
        SoundInput(Propagation, std::unique_ptr<StateAllocator> mainStateAllocator);

        void setSource(SoundSource*);

        SoundSource* getSource() noexcept;
        const SoundSource* getSource() const noexcept;

        // TODO
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
