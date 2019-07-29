#pragma once

#include <SoundChunk.hpp>
#include <SoundNode.hpp>
#include <SoundState.hpp>
#include <SoundInput.hpp>

#include <vector>

namespace flo {

    class SoundSource : public SoundNode {
    public:

        /**
         * Produces the next chunk of sound. The SoundSource's state corresponding
         * to the given context may be updated.
         */
        virtual void getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* context) = 0;


        //class Decorator : public SoundNode::Decorator {
            // TODO: ???
        //};
    };

} // namespace flo
