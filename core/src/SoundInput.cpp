#include <Flosion/Core/SoundInput.hpp>

#include <Flosion/Core/SoundSource.hpp>

namespace flo {

    SoundInput::SoundInput() noexcept
        : m_source(nullptr) {
    }

    SoundInput::~SoundInput(){
        notifyReactors(&SoundInputReactor::onDestroySoundInput);
        setSource(nullptr);
    }

    void SoundInput::setSource(SoundSource* source){
        auto lock = acquireLock();
        if (m_source){

            m_source->notifyReactors(&SoundSourceReactor::beforeInputRemoved, const_cast<const SoundInput*>(this));
            notifyReactors(&SoundInputReactor::beforeSourceRemoved, const_cast<const SoundSource*>(m_source));

            assert(std::count(m_source->m_inputs.begin(), m_source->m_inputs.end(), this) == 1);
            auto it = std::find(m_source->m_inputs.begin(), m_source->m_inputs.end(), this);
            assert(it != m_source->m_inputs.end());
            m_source->m_inputs.erase(it);

            removeDependency(m_source);
        }
        m_source = source;
        if (m_source){
            addDependency(m_source);

            assert(std::count(m_source->m_inputs.begin(), m_source->m_inputs.end(), this) == 0);
            m_source->m_inputs.push_back(this);

            notifyReactors(&SoundInputReactor::afterSourceAdded, const_cast<const SoundSource*>(m_source));
            m_source->notifyReactors(&SoundSourceReactor::afterInputAdded, const_cast<const SoundInput*>(this));
        }
    }

    SoundSource* SoundInput::getSource() noexcept {
        return m_source;
    }

    const SoundSource* SoundInput::getSource() const noexcept {
        return m_source;
    }

} // namespace flo
