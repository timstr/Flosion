#pragma once

#include <Flosion/Core/SoundInput.hpp>
#include <Flosion/Core/SoundState.hpp>

#include <cassert>

namespace flo {

    template<typename SoundStateType>
    inline void ControlledSoundSource<SoundStateType>::getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* dependentState){
        assert(dependent->hasDirectDependency(this));
		auto ownState = this->getState(dependent, dependentState);
        this->renderNextChunk(chunk, ownState);
        auto os = static_cast<SoundState*>(ownState);
        os->m_coarseTime += SoundChunk::size;
		os->m_fineTime = 0;
    }

    template<typename SoundStateType>
    inline void UncontrolledSoundSource<SoundStateType>::getNextChunkFor(SoundChunk& chunk, const SoundInput* dependent, const SoundState* dependentState){
        assert(dependent->hasDirectDependency(this));
        auto ownState = this->getState(dependent, dependentState);
        this->renderNextChunk(chunk, ownState);
		auto os = static_cast<SoundState*>(ownState);
        os->m_coarseTime += SoundChunk::size;
		os->m_fineTime = 0;
    }

} // namespace flo
