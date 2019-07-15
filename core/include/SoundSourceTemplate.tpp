#pragma once

#include <SoundSourceTemplate.hpp>
#include <StateAllocator.hpp>

namespace flo {

    template<typename SoundStateType>
    SoundSourceTemplate<SoundStateType>::SoundSourceTemplate(Controllability controllability, TimeSync timeSync)
        : SoundSource(controllability, timeSync, std::make_unique<ConcreteStateAllocator<SoundStateType>>()) {
    
        static_assert(std::is_base_of_v<SoundState, SoundStateType>);
    }

    template<typename SoundStateType>
    void SoundSourceTemplate<SoundStateType>::getNextChunkFor(SoundChunk&, const SoundState*){
        // TODO
    }
    
    template<typename SoundStateType>
    void SoundSourceTemplate<SoundStateType>::addStateFor(const SoundNode* node, const SoundState* state){
        // TODO
    }

    template<typename SoundStateType>
    void SoundSourceTemplate<SoundStateType>::removeStateFor(const SoundNode* node, const SoundState* state){
        // TODO
    }
    
    template<typename SoundStateType>
    void SoundSourceTemplate<SoundStateType>::resetStateFor(const SoundNode* node, const SoundState* state){
        // TODO
    }

    template<typename SoundStateType>
    void SoundSourceTemplate<SoundStateType>::onStateMoved(const SoundNode* node, const SoundState* from, const SoundState* to){
        // TODO
    }

} // namespace flo
