#pragma once
#include <SFML\Audio.hpp>
#include "musical.h"

namespace musical {

	struct DAC : private sf::SoundStream {
		DAC(){
			initialize(2, SFREQ);
		}

		void play(){
			SoundStream::play();
		}
		void pause(){
			SoundStream::stop();
		}
		void reset(){
			input.reset();
		}

		bool isPlaying(){
			return (SoundStream::getStatus() == SoundStream::Playing);
		}

		// returns the average amplitude of the current chunk, intended for aesthetic use
		Sample getCurrentAmp(){
			if (isPlaying()){
				double lsum = 0;
				double rsum = 0;
				for (int i = 0; i < CHUNK_SIZE; i++){
					lsum += abs(inbuffer[i].l);
					rsum += abs(inbuffer[i].r);
				}
				return Sample(lsum / CHUNK_SIZE, rsum / CHUNK_SIZE);
			} else {
				return Sample(0, 0);
			}
		}

		SoundResult input;

		private:

		Sample inbuffer[CHUNK_SIZE];
		int16_t outbuffer[CHUNK_SIZE * 2];

		bool onGetData(Chunk& data) override {

			input.getNextChunk(inbuffer);

			for (int i = 0; i < CHUNK_SIZE; i++){
				outbuffer[2 * i]		= inbuffer[i].l * INT16_MAX;
				outbuffer[2 * i + 1]	= inbuffer[i].r * INT16_MAX;
			}

			data.samples = outbuffer;
			data.sampleCount = CHUNK_SIZE * 2;

			return true;
		}

		void onSeek(sf::Time timeOffset) override {

		}
	};
}