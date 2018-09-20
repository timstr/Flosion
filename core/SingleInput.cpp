#include "SingleInput.h"
#include "SoundSource.h"

namespace musical {

	SingleInput::SingleInput(Stateful* parent) noexcept :
		SoundInput(parent) {
		
	}

	SingleInput::~SingleInput() noexcept{
		setSource(nullptr);
	}

	void SingleInput::getNextChunk(SoundChunk& chunk, const State* state) noexcept {
		if (m_source){
			try {
				m_source->getNextChunk(chunk, state, this);
				return;
			} catch (...) { }
		}
		chunk.silence();
	}

	void SingleInput::addState(const State* parent_state, const Stateful* dependant) noexcept {
		if (m_source){
			m_source->addState(parent_state, dependant);
		}
	}

	void SingleInput::removeState(const State* parent_state, const Stateful* dependant) noexcept {
		if (m_source){
			m_source->removeState(parent_state, dependant);
		}
	}

	void SingleInput::resetState(const State* parent_state, const Stateful* dependant) noexcept {
		if (m_source){
			m_source->resetState(parent_state, dependant);
		}
	}

	void SingleInput::addAllStatesTo(Stateful* dependency, const Stateful* via) const noexcept {
		assert(via == this);
		for (const auto& d : dependants){
			d->addAllStatesTo(dependency, this);
		}
	}

	void SingleInput::removeAllStatesFrom(Stateful* dependency, const Stateful* via) const noexcept {
		assert(via == this);
		for (const auto& d : dependants){
			d->removeAllStatesFrom(dependency, this);
		}
	}

	std::size_t SingleInput::numStates() const noexcept {
		std::size_t ret = 0;
		for (const auto& d : dependants){
			ret += d->numStates();
		}
		return ret;
	}

} // namespace musical