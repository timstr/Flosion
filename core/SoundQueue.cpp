#include "SoundQueue.h"

namespace musical {

	SoundQueue::SoundQueue(size_t num_samples) noexcept :
		chunks((num_samples / CHUNK_SIZE) + 1),
		chunk_index(0),
		offset(0),
		nsamples(num_samples) {

	};

	// advance queue, bring in silence
	void SoundQueue::advance(std::size_t num_samples) noexcept {
		offset += num_samples;
		while (offset >= CHUNK_SIZE){
			chunk_index = (chunk_index + 1) % chunks.size();
			offset -= CHUNK_SIZE;
		}
	}

	// advance queue, bring in data from sound input
	void SoundQueue::advance(std::size_t num_samples, SingleInput& input, const State* input_state) noexcept {
		offset += num_samples;
		while (offset >= CHUNK_SIZE){
			input.getNextChunk(chunks[chunk_index], input_state);
			chunk_index = (chunk_index + 1) % chunks.size();
			offset -= CHUNK_SIZE;
		}
	}

	void SoundQueue::prefill(SingleInput& input, const State* state) noexcept {
		clear();
		input.getNextChunk(chunks.front(), state);
	}

	void SoundQueue::clear() noexcept {
		chunks.front().silence();
		chunk_index = 0;
		offset = 0;
	}

	Sample& SoundQueue::operator[](std::size_t index) noexcept {
		std::size_t i = index + offset;
		std::size_t c = ((i / CHUNK_SIZE) + chunk_index) % chunks.size();
		std::size_t s = i % CHUNK_SIZE;
		return chunks[c][s];
	}

	std::size_t SoundQueue::size() const noexcept {
		return nsamples;
	}

} // namespace musical