#pragma once

// TODO: make sure to DELETE the "#include <SoundSourceTemplate.hpp>" that Visual Studio inserts here >:(

#include <SoundState.hpp>

#include <cassert>

namespace flo {

    template<typename SoundStateType>
    inline void ControlledSoundSource<SoundStateType>::getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* dependentState){
        assert(dependent->hasDirectDependency(this));
		auto ownState = getState(dependent, dependentState);
        renderNextChunk(chunk, ownState);
        auto os = static_cast<SoundState*>(ownState);
        os->m_coarseTime += SoundChunk::size;
		os->m_fineTime = 0;
    }

    template<typename SoundStateType>
    inline void UncontrolledSoundSource<SoundStateType>::getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* dependentState){
        assert(dependent->hasDirectDependency(this));
        auto ownState = getState(dependent, dependentState);
        renderNextChunk(chunk, ownState);
		auto os = static_cast<SoundState*>(ownState);
        os->m_coarseTime += SoundChunk::size;
		os->m_fineTime = 0;
    }

} // namespace flo
