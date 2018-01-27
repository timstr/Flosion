#pragma once

#include "musical.h"
#include <vector>
#include "SoundSource.h"

namespace musical {

	// TODO: add 'prefill' function that fills queue with data to avoid initial delay

	struct SoundQueue {
		SoundQueue(size_t num_samples) : nsamples(num_samples) {
			offset = 0;
			chunk_index = 0;
			size_t nchunks = (num_samples / CHUNK_SIZE) + 1;
			chunks.resize(nchunks);
			for (int i = 0; i < nchunks; i++){
				chunks[i] = new Chunk();
			}
		};
		~SoundQueue(){
			for (int i = 0; i < chunks.size(); i++){
				delete chunks[i];
			}
			chunks.clear();
		}

		// advance queue, bring in silence
		inline void advance(size_t num_samples){
			offset += num_samples;
			while (offset >= CHUNK_SIZE){
				chunks[chunk_index]->clear();
				chunk_index = (chunk_index + 1) % chunks.size();
				offset -= CHUNK_SIZE;
			}
		}

		// advance queue, bring in data from sound input
		inline void advance(size_t num_samples, SingleInput& input, State* input_state){
			offset += num_samples;
			while (offset >= CHUNK_SIZE){
				input.getNextChunk(chunks[chunk_index]->data, input_state);
				chunk_index = (chunk_index + 1) % chunks.size();
				offset -= CHUNK_SIZE;
			}
		}

		void clear(){
			for (int i = 0; i < chunks.size(); i++){
				chunks[i]->clear();
			}
		}

		inline Sample& operator[](size_t index){
			size_t i = index + offset;
			size_t c = ((i / CHUNK_SIZE) + chunk_index) % chunks.size();
			size_t s = i % CHUNK_SIZE;
			return chunks[c]->data[s];
		}

		size_t size() const {
			return nsamples;
		}

		private:

		struct Chunk {
			Chunk(){
				clear();
			}

			void clear(){
				for (int i = 0; i < CHUNK_SIZE; i++){
					data[i] = Sample(0, 0);
				}
			}

			Buffer data;
		};

		std::vector<Chunk*> chunks;

		size_t offset;
		size_t chunk_index;
		const size_t nsamples;
	};

};