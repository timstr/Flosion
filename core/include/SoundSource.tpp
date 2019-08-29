namespace flo {

    template<typename SoundNodeType>
    inline SoundNumberSource<SoundNodeType>::SoundNumberSource(SoundNodeType* owner) noexcept {
        setStateOwner(owner);
    }

    template<typename SoundNodeType>
    inline SoundNodeType* SoundNumberSource<SoundNodeType>::getOwner() noexcept {
        return reinterpret_cast<SoundNodeType*>(getStateOwner());
    }

    template<typename SoundNodeType>
    inline const SoundNodeType* SoundNumberSource<SoundNodeType>::getOwner() const noexcept {
        return reinterpret_cast<const SoundNodeType*>(getStateOwner());
    }

    template<typename SoundNodeType>
    inline double SoundNumberSource<SoundNodeType>::evaluate(const SoundState* context) const noexcept {
        auto curr = context;
        while (curr){
            const auto owner = curr->getOwner();
            if (owner == getStateOwner()){
                return evaluate(reinterpret_cast<const StateType*>(curr), context);
            }
            curr = curr->getDependentState();
        }
        assert(false);
        return 0.0;
    }

} // namespace flo 
