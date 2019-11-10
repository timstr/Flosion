#pragma once

#include <Flosion/Core/SoundSource.hpp>
#include <Flosion/Core/SoundState.hpp>

#include <cassert>

namespace flo {

    template<typename SoundStateType>
    inline const SoundStateType* ControlledSoundSource<SoundStateType>::findOwnState(const SoundState* context) const noexcept {
        assert(context);
        while (context->getOwner() != this){
            context = context->getDependentState();
        }
        assert(context);
        auto derived = static_cast<const SoundStateType*>(context);
        assert(dynamic_cast<const SoundStateType*>(context) == derived);
        return derived;
    }

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

    template<typename SoundStateType>
    inline const SoundStateType* UncontrolledSoundSource<SoundStateType>::findOwnState(const SoundState* context) const noexcept {
        assert(context);
        while (context->getOwner() != this){
            context = context->getDependentState();
        }
        assert(context);
        auto derived = static_cast<const SoundStateType*>(context);
        assert(dynamic_cast<const SoundStateType*>(context) == derived);
        return derived;
    }

} // namespace flo
