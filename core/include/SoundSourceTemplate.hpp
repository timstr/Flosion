#pragma once

#include <SoundSource.hpp>
#include <StateTable.hpp>

#include <map>

namespace flo {

    class SoundInput;

    template<typename SoundStateType>
    class SoundSourceTemplate : public Singular<SoundSource, SoundStateType> {
    public:

        virtual void renderNextChunk(SoundChunk& chunk, SoundStateType* state) = 0;

    private:
        void getNextChunkFor(SoundChunk&, const SoundState*) override final;

    };

} // namespace flo

#include <SoundSourceTemplate.tpp>
