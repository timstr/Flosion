#pragma once

#include <SFML\Audio.hpp>
#include "SoundResult.h"
#include "musical.h"
#include <thread>

// TODO: very strange bugs here when musical::CHUNK_SIZE != 1024

namespace musical {

	struct DAC : private sf::SoundStream {
		DAC();
		~DAC();

		void play();
		void pause();
		void reset();

		bool isPlaying() const;

		// returns the average amplitude of the current chunk, intended for aesthetic use
		Sample getCurrentAmp() const;

		SoundResult input;

		private:

		std::vector<Sample> inbuffer;
		std::vector<int16_t> outbuffer;

		// sf::SoundStream overrides

		bool onGetData(Chunk& data) override;

		void onSeek(sf::Time timeOffset) override;
	};
}