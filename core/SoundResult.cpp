#pragma once

#include "SoundResult.h"
//#include <cassert>
#define assert(x) do { if (!static_cast<bool>(x)) { throw std::runtime_error("Assertion failure!"); } } while (false);

namespace flo {

	SoundResult::SoundResult() NOEXCEPT_IF_I_SAY_SO :
		input(this),
		state(nullptr, this) {

		input.addState(&state, this);
	}

	void SoundResult::getNextChunk(SoundChunk& chunk) NOEXCEPT_IF_I_SAY_SO {
		input.getNextChunk(chunk, &state);
	}

	void SoundResult::reset() NOEXCEPT_IF_I_SAY_SO {
		input.resetState(&state, this);
	}

	void SoundResult::setSource(SoundSource* source) NOEXCEPT_IF_I_SAY_SO{
		input.setSource(source);
	}

	std::size_t SoundResult::getTime() const NOEXCEPT_IF_I_SAY_SO {
		return state.getGlobalTime();
	}

	void SoundResult::addState(const State* parent_state, const Stateful* dependant) NOEXCEPT_IF_I_SAY_SO {
		assert(false);
	}

	void SoundResult::removeState(const State* parent_state, const Stateful* dependant) NOEXCEPT_IF_I_SAY_SO {
		assert(false);
	}

	void SoundResult::resetState(const State* parent_state, const Stateful* dependant) NOEXCEPT_IF_I_SAY_SO {
		assert(false);
	}

	void SoundResult::addAllStatesTo(Stateful* dependency) const NOEXCEPT_IF_I_SAY_SO {
		dependency->addState(&state, this);
	}

	void SoundResult::removeAllStatesFrom(Stateful* dependency) const NOEXCEPT_IF_I_SAY_SO {
		dependency->removeState(&state, this);
	}

	std::size_t SoundResult::numStates() const NOEXCEPT_IF_I_SAY_SO {
		return 1;
	}

}