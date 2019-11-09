namespace flo {

    template<typename SoundNodeType>
    inline SoundNumberSource<SoundNodeType>::SoundNumberSource(SoundNodeType* owner) noexcept {
        setStateOwner(owner);
    }

    template<typename SoundNodeType>
    inline SoundNodeType* SoundNumberSource<SoundNodeType>::getOwner() noexcept {
        auto o = this->getStateOwner();
        assert(o);
        auto od = static_cast<SoundNodeType*>(o);
        assert(dynamic_cast<SoundNodeType*>(o) == od);
        return od;
    }

    template<typename SoundNodeType>
    inline const SoundNodeType* SoundNumberSource<SoundNodeType>::getOwner() const noexcept {
        auto o = this->getStateOwner();
        assert(o);
        auto od = static_cast<const SoundNodeType*>(o);
        assert(dynamic_cast<const SoundNodeType*>(o) == od);
        return od;
    }

    template<typename SoundNodeType>
    inline double SoundNumberSource<SoundNodeType>::evaluate(const SoundState* context) const noexcept {
        auto curr = context;
        while (curr){
            const auto owner = curr->getOwner();
            if (owner == getStateOwner()){
                auto d = static_cast<const StateType*>(curr);
                assert(dynamic_cast<const StateType*>(curr) == d);
                return evaluate(d, context);
            }
            curr = curr->getDependentState();
        }
        assert(false);
        return 0.0;
    }

} // namespace flo 
