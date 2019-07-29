#pragma once

#include <SoundSource.hpp>
#include <StateTable.hpp>

namespace flo {

    class SoundInput;

    template<typename SoundStateType>
    class ControlledSoundSource : public Singular<SoundSource, SoundStateType> {
    public:

        virtual void renderNextChunk(SoundChunk& chunk, SoundStateType* state) = 0;

    private:
        void getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* dependentState) override final;

    };

    template<typename SoundStateType>
    class UncontrolledSoundSource : public Uncontrolled<SoundSource, SoundStateType> {
    public:

        virtual void renderNextChunk(SoundChunk& chunk, SoundStateType* state) = 0;

    private:
        void getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* dependentState) override final;

    };


} // namespace flo

#include <SoundSourceTemplate.tpp>
