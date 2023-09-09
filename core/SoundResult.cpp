#include "SoundResult.h"
#include <cassert>

namespace flo {

	SoundResult::SoundResult() noexcept :
		input(this),
		state(nullptr, this) {

		input.addState(&state, this);
	}

	void SoundResult::getNextChunk(SoundChunk& chunk) noexcept {
		input.getNextChunk(chunk, &state);
	}

	void SoundResult::reset() noexcept {
		input.resetState(&state, this);
	}

	void SoundResult::setSource(SoundSource* source) noexcept{
		input.setSource(source);
	}

	std::size_t SoundResult::getTime() const noexcept {
		return state.getGlobalTime();
	}

	void SoundResult::addState(const State* parent_state, const Stateful* dependant) noexcept {
		assert(false);
	}

	void SoundResult::removeState(const State* parent_state, const Stateful* dependant) noexcept {
		assert(false);
	}

	void SoundResult::resetState(const State* parent_state, const Stateful* dependant) noexcept {
		assert(false);
	}

	void SoundResult::addAllStatesTo(Stateful* dependency) const noexcept {
		dependency->addState(&state, this);
	}

	void SoundResult::removeAllStatesFrom(Stateful* dependency) const noexcept {
		dependency->removeState(&state, this);
	}

	std::size_t SoundResult::numStates() const noexcept {
		return 1;
	}

}