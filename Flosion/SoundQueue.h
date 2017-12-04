#pragma once

#include "musical.h"
#include "Sample.h"
#include <vector>
#include "SoundSource.h"

namespace musical {

	/*
	Things for a SoundQueue to do:
	- copy audio data to back
	- copy audio data from front
	- advance by specified number of samples
	*/

	struct SoundQueue {
		SoundQueue(size_t num_samples) : nsamples(num_samples) {
			offset = 0;
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
				Chunk* ch = chunks[0];
				chunks.erase(chunks.begin());
				chunks.push_back(ch);
				ch->clear();
				offset -= CHUNK_SIZE;
			}
		}

		// advance queue, bring in data from sound input
		inline void advance(size_t num_samples, SingleInput& input, State* input_state){
			offset += num_samples;
			while (offset >= CHUNK_SIZE){
				Chunk* ch = chunks[0];
				chunks.erase(chunks.begin());
				chunks.push_back(ch);
				input.getNextChunk(ch->data, input_state);
				offset -= CHUNK_SIZE;
			}
		}

		inline Sample& operator[](size_t index){
			size_t i = index + offset;
			size_t c = i / CHUNK_SIZE;
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

			Sample data[CHUNK_SIZE];
		};

		std::vector<Chunk*> chunks;

		size_t offset;
		const size_t nsamples;
	};

};