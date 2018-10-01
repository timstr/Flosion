#pragma once

#include "DAC.h"

namespace flo {

	DAC::DAC(){
		initialize(2, SFREQ);
	}
	DAC::~DAC(){
		pause();
	}

	void DAC::play(){
		SoundStream::play();
	}
	void DAC::pause(){
		if (isPlaying()){
			SoundStream::stop();
		}
	}
	void DAC::reset(){
		input.reset();
	}

	bool DAC::isPlaying() const {
		return (SoundStream::getStatus() == SoundStream::Playing);
	}

	// returns the average amplitude of the current chunk, intended for aesthetic use
	Sample DAC::getCurrentAmp() const {
		if (isPlaying()){
			double lsum = 0;
			double rsum = 0;
			for (int i = 0; i < CHUNK_SIZE; i++){
				lsum += abs(inbuffer[i].l);
				rsum += abs(inbuffer[i].r);
			}
			return Sample((float)(lsum / CHUNK_SIZE), (float)(rsum / CHUNK_SIZE));
		} else {
			return Sample(0, 0);
		}
	}

	bool DAC::onGetData(Chunk& data){
		input.getNextChunk(inbuffer);

		// format and write the chunk data
		for (int i = 0; i < CHUNK_SIZE; i++){
			outbuffer.at(2 * i)		= uint32_t(std::min(std::max(inbuffer[i].l, -1.0f), 1.0f) * INT16_MAX);
			outbuffer.at(2 * i + 1)	= uint32_t(std::min(std::max(inbuffer[i].r, -1.0f), 1.0f) * INT16_MAX);
		}
		data.samples = outbuffer.data();
		data.sampleCount = CHUNK_SIZE * 2;

		return true;
	}

	void DAC::onSeek(sf::Time timeOffset){

	}

}