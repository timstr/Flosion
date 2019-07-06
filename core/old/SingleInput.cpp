#include "SingleInput.h"
#include "SoundSource.h"

namespace flo {

	SingleInput::SingleInput(Stateful* parent) noexcept :
		SoundInput(parent) {
		
	}

	SingleInput::~SingleInput() noexcept{
		setSource(nullptr);
	}

	void SingleInput::getNextChunk(SoundChunk& chunk, const State* state) noexcept {
		if (m_source){
			m_source->getNextChunk(chunk, state, this);
		} else {
			chunk.silence();
		}
	}

	void SingleInput::addState(const State* parent_state, const Stateful* dependant) noexcept {
		assert(dependant == m_parent);
		assert(std::find(dependants.begin(), dependants.end(), dependant) != dependants.end());
		assert(m_statemap.find(parent_state) == m_statemap.end());
		m_statemap.insert(parent_state);
		if (m_source){
			m_source->addState(parent_state, this);
		}
	}

	void SingleInput::removeState(const State* parent_state, const Stateful* dependant) noexcept {
		assert(dependant == m_parent);
		assert(std::find(dependants.begin(), dependants.end(), dependant) != dependants.end());
		auto it = m_statemap.find(parent_state);
		assert(it != m_statemap.end());
		m_statemap.erase(it);
		if (m_source){
			m_source->removeState(parent_state, this);
		}
	}

	void SingleInput::resetState(const State* parent_state, const Stateful* dependant) noexcept {
		assert(dependant == m_parent);
		assert(std::find(dependants.begin(), dependants.end(), dependant) != dependants.end());
		assert(m_statemap.find(parent_state) != m_statemap.end());
		if (m_source){
			m_source->resetState(parent_state, this);
		}
	}

	void SingleInput::addAllStatesTo(Stateful* dependency) const noexcept {
		assert(std::find(dependencies.begin(), dependencies.end(), dependency) != dependencies.end());
		for (const auto& state : m_statemap) {
			dependency->addState(state, this);
		}
	}

	void SingleInput::removeAllStatesFrom(Stateful* dependency) const noexcept {
		assert(std::find(dependencies.begin(), dependencies.end(), dependency) != dependencies.end());
		for (const auto& state : m_statemap) {
			dependency->removeState(state, this);
		}
	}

	std::size_t SingleInput::numStates() const noexcept {
		return m_statemap.size();
	}

} // namespace flo