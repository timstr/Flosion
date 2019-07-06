#pragma once

#include <SoundSource.hpp>

namespace flo::sound {

    template<typename SoundStateType>
    class SoundSourceTemplate : public SoundSource {
    public:
        virtual void renderNextChunk(SoundChunk& chunk, SoundStateType* state) = 0;

    private:
        void getNextChunk(SoundChunk&, const SoundState*) override;
    };

} // namespace flo::sound
