namespace flo {

    template<typename SoundSourceType>
    inline SoundSourceType& ConcreteSoundState<SoundSourceType>::getOwner() noexcept {
        auto o = this->SoundState::getOwner();
        assert(o);
        auto od = static_cast<SoundSourceType*>(o);
        assert(dynamic_cast<SoundSourceType*>(o) == od);
        return *od;
    }

    template<typename SoundSourceType>
    inline const SoundSourceType& ConcreteSoundState<SoundSourceType>::getOwner() const noexcept {
        auto o = this->SoundState::getOwner();
        assert(o);
        auto od = static_cast<const SoundSourceType*>(o);
        assert(dynamic_cast<const SoundSourceType*>(o) == od);
        return *od;
    }
} // namespace flo
