#include "SoundInput.h"
#include "SoundSource.h"

namespace musical {

	SoundInput::SoundInput(Stateful* _parent) noexcept :
		m_parent(_parent),
		m_source(nullptr) {
		assert(m_parent);
		m_parent->addDependency(this);
	}

	SoundInput::~SoundInput() noexcept {
		m_parent->removeDependency(this);
	}

	SoundSource* SoundInput::getSource() const noexcept {
		return m_source;
	}

	void SoundInput::setSource(SoundSource * _source) noexcept {
		if (m_source){
			auto& dsts = m_source->destinations;
			dsts.erase(std::remove(dsts.begin(), dsts.end(), this), dsts.end());
			removeDependency(m_source);
			removeAllStatesFrom(m_source, this);
		}

		m_source = _source;

		if (m_source){
			addAllStatesTo(m_source, this);
			addDependency(m_source);
			m_source->destinations.push_back(this);
		}
	}

} // namespace musical