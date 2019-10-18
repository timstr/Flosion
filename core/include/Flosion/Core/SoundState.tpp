namespace flo {
    template<typename SoundSourceType>
    inline SoundSourceType& ConcreteSoundState<SoundSourceType>::getOwner() noexcept {
        // TODO: safety assertions
        return reinterpret_cast<SoundSourceType&>(*SoundState::getOwner());
    }

    template<typename SoundSourceType>
    inline const SoundSourceType& ConcreteSoundState<SoundSourceType>::getOwner() const noexcept {
        // TODO: safety assertions
        return reinterpret_cast<SoundSourceType&>(*SoundState::getOwner());
    }
} // namespace flo
