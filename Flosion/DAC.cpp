#pragma once

#include "DAC.h"

namespace musical {

	DAC::DAC(){
		initialize(2, SFREQ);
		_play = false;
		render_chunk = false;
		chunk_avail = false;
	}
	DAC::~DAC(){
		pause();
	}

	void DAC::play(){
		// set up flags
		_play = true;
		render_chunk = true;
		chunk_avail = false;
		// spawn the worker thread
		worker = std::thread(renderAudio, this);
		// begin getting chunks
		SoundStream::play();
	}
	void DAC::pause(){
		// signal the worker thread to stop
		_play = false;
		// terminate the worker thread
		if (worker.joinable()){
			worker.join();
		}
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

	void DAC::renderAudio(DAC* dac){
		while (dac->_play){
			// do the work
			dac->input.getNextChunk(dac->inbuffer);
			// signal that the next chunk is ready
			dac->render_chunk = false;
			dac->chunk_avail = true;
			// wait for the next chunk to be needed
			while (dac->_play && !dac->render_chunk){

			}
		}
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

		// ensure the worker thread is running
		if (!worker.joinable()){
			throw std::runtime_error("The audio rendering thread does not appear to be running");
		}

		while (!chunk_avail && _play){
			// wait for the worker thread to render the next chunk
		}

		// format and write the chunk data
		for (int i = 0; i < CHUNK_SIZE; i++){
			outbuffer[2 * i]		= inbuffer[i].l * INT16_MAX;
			outbuffer[2 * i + 1]	= inbuffer[i].r * INT16_MAX;
		}
		data.samples = outbuffer;
		data.sampleCount = CHUNK_SIZE * 2;

		// signal the worker thread
		chunk_avail = false;
		render_chunk = true;

		return true;
	}

	void DAC::onSeek(sf::Time timeOffset){

	}

}