#pragma once

// TODO: make sure to DELETE the "#include <SoundSourceTemplate.hpp>" that Visual Studio inserts here >:(

namespace flo {

    template<typename SoundStateType>
    inline void ControlledSoundSource<SoundStateType>::getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* dependentState){
        renderNextChunk(chunk, getState(dependent, dependentState));
    }

    template<typename SoundStateType>
    inline void UncontrolledSoundSource<SoundStateType>::getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* dependentState){
        renderNextChunk(chunk, getState(dependent, dependentState));
    }

} // namespace flo
