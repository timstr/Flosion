#pragma once

#include "Sample.h"
#include "SoundSource.h"
#include "SingleInput.h"
#include <vector>
#include <memory>

namespace flo {

	// a SoundQueue can be used with a SingleInput to create a queue of
	// sound data, which is useful for cases where more sound data than
	// that of a single SoundChunk is needed.
	// a SoundQueue does not own the SingleInput it uses so that it can
	// be used in the State of a SoundSource
	struct SoundQueue {
		// construct a queue with capacity for 'num_samples' samples
		SoundQueue(size_t num_samples) NOEXCEPT_IF_I_SAY_SO;

		// advance queue, bring in silence
		void advance(std::size_t num_samples) NOEXCEPT_IF_I_SAY_SO;

		// advance queue, bring in data from sound input
		// to be used in place of 'input.getNextChunk(..., state);`
		void advance(std::size_t num_samples, SingleInput& input, const State* state) NOEXCEPT_IF_I_SAY_SO;

		// initializes the queue with a single SoundChunk's worth of sound
		// This should be done at the beginning of reading from the SoundInput
		// to prevent a delay of one SoundChunk, which will happen otherwise.
		void prefill(SingleInput& input, const State* state) NOEXCEPT_IF_I_SAY_SO;

		// silence and reset the queue
		void clear() NOEXCEPT_IF_I_SAY_SO;

		Sample& operator[](std::size_t index) NOEXCEPT_IF_I_SAY_SO;

		std::size_t size() const NOEXCEPT_IF_I_SAY_SO;

		private:

		std::vector<SoundChunk> chunks;

		std::size_t offset;
		std::size_t chunk_index;
		const std::size_t nsamples;
	};

};