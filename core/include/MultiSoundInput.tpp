namespace flo {

    template<typename StateType, typename KeyType>
    inline void MultiSoundInput<StateType, KeyType>::getNextChunkFor(SoundChunk& chunk, const SoundNode* node, const SoundState* state, const KeyType& key){
        assert(node->hasDirectDependency(this));
        if (getSource()){
            getSource()->getNextChunkFor(chunk, this, getState(node, state, key));
        } else {
            chunk.silence();
        }
    }

} // namespace flo 
