#include <Flosion/Core/SoundInput.hpp>

#include <Flosion/Core/SoundSource.hpp>

namespace flo {

    SoundInput::SoundInput() noexcept
        : m_source(nullptr) {
    }

    void SoundInput::setSource(SoundSource* source){
        auto lock = getScopedWriteLock();
        if (m_source){
            removeDependency(m_source);
        }
        m_source = source;
        if (m_source){
            addDependency(m_source);
        }
    }

    SoundSource* SoundInput::getSource() noexcept {
        return m_source;
    }

    const SoundSource* SoundInput::getSource() const noexcept {
        return m_source;
    }

} // namespace flo
