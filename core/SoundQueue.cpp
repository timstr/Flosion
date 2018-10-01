#include "SoundQueue.h"

namespace flo {

	SoundQueue::SoundQueue(size_t num_samples) NOEXCEPT_IF_I_SAY_SO :
		chunks((num_samples / CHUNK_SIZE) + 1),
		chunk_index(0),
		offset(0),
		nsamples(num_samples) {

	};

	// advance queue, bring in silence
	void SoundQueue::advance(std::size_t num_samples) NOEXCEPT_IF_I_SAY_SO {
		offset += num_samples;
		while (offset >= CHUNK_SIZE){
			chunk_index = (chunk_index + 1) % chunks.size();
			offset -= CHUNK_SIZE;
		}
	}

	// advance queue, bring in data from sound input
	void SoundQueue::advance(std::size_t num_samples, SingleInput& input, const State* input_state) NOEXCEPT_IF_I_SAY_SO {
		offset += num_samples;
		while (offset >= CHUNK_SIZE){
			input.getNextChunk(chunks[chunk_index], input_state);
			chunk_index = (chunk_index + 1) % chunks.size();
			offset -= CHUNK_SIZE;
		}
	}

	void SoundQueue::prefill(SingleInput& input, const State* state) NOEXCEPT_IF_I_SAY_SO {
		clear();
		input.getNextChunk(chunks.front(), state);
	}

	void SoundQueue::clear() NOEXCEPT_IF_I_SAY_SO {
		chunks.front().silence();
		chunk_index = 0;
		offset = 0;
	}

	Sample& SoundQueue::operator[](std::size_t index) NOEXCEPT_IF_I_SAY_SO {
		std::size_t i = index + offset;
		std::size_t c = ((i / CHUNK_SIZE) + chunk_index) % chunks.size();
		std::size_t s = i % CHUNK_SIZE;
		return chunks[c][s];
	}

	std::size_t SoundQueue::size() const NOEXCEPT_IF_I_SAY_SO {
		return nsamples;
	}

} // namespace flo