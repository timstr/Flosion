#include <SoundInput.hpp>

#include <SoundSource.hpp>

namespace flo {

    void SoundInput::setSource(SoundSource* source){
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
