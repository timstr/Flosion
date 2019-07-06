#include "SoundInput.h"
#include "SoundSource.h"

namespace flo {

	SoundInput::SoundInput(Stateful* _parent) noexcept :
		m_parent(_parent),
		m_source(nullptr) {

		assert(m_parent);
		m_parent->addDependency(this);
	}

	SoundInput::~SoundInput() noexcept {
		m_parent->removeDependency(this);
		// Note that setSource() is not called here since it would result in nonesense
		// virtual function calls. setSource(nullptr) should be performed in concrete
		// SoundInput classes
	}

	SoundSource* SoundInput::getSource() const noexcept {
		return m_source;
	}

	void SoundInput::setSource(SoundSource* _source) noexcept {
		if (m_source){
			removeAllStatesFrom(m_source);
			removeDependency(m_source);
			auto& dsts = m_source->destinations;
			dsts.erase(std::remove(dsts.begin(), dsts.end(), this), dsts.end());
		}

		m_source = _source;

		if (m_source){
			m_source->destinations.push_back(this);
			addDependency(m_source);
			addAllStatesTo(m_source);
		}
	}

} // namespace flo