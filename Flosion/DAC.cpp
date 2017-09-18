#pragma once

#include "DAC.h"

namespace musical {

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
			return Sample(lsum / CHUNK_SIZE, rsum / CHUNK_SIZE);
		} else {
			return Sample(0, 0);
		}
	}

	bool DAC::onGetData(Chunk& data){
		input.getNextChunk(inbuffer);

		// format and write the chunk data
		for (int i = 0; i < CHUNK_SIZE; i++){
			outbuffer[2 * i]		= inbuffer[i].l * INT16_MAX;
			outbuffer[2 * i + 1]	= inbuffer[i].r * INT16_MAX;
		}
		data.samples = outbuffer;
		data.sampleCount = CHUNK_SIZE * 2;

		return true;
	}

	void DAC::onSeek(sf::Time timeOffset){

	}

}