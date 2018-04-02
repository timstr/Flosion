#pragma once

#include "SoundSource.h"
#include <SFML\Audio.hpp>
#include <functional>
#include <thread>

namespace musical {

	struct AudioState : State {
		using State::State;

		void reset() override {
			pos = 0;
			pos_carryover = 0;
		}
		uint64_t pos;
		float pos_carryover;
	};

	struct Audio : SoundSourceTemplate<AudioState> {
		Audio(){
			loaded = true;
		}
		~Audio(){
			if (loader_thread.joinable()){
				loader_thread.join();
			}
		}

		void loadFromFile(std::string filename){
			if (loader_thread.joinable()){
				loader_thread.join();
			}
			loaded = false;
			std::function<void(Audio*,std::string)> loadFile = [=](Audio* audio, std::string filename){
				audio->soundbuffer.loadFromFile(filename);
				audio->loaded = true;
			};
			loader_thread = std::thread(loadFile, this, filename);
		}

		void renderChunk(Buffer& buffer, AudioState* state) override {
			if (soundbuffer.getSampleCount() == 0 || !loaded){
				for (int i = 0; i < CHUNK_SIZE; i++){
					buffer[i] = {0, 0};
				}
			} else {
				int channels = soundbuffer.getChannelCount();
				int sfreq = soundbuffer.getSampleRate();
				for (int i = 0; i < CHUNK_SIZE; i++){
					if (state->pos > soundbuffer.getSampleCount()){
						state->pos = 0;
					}

					buffer[i].l = soundbuffer.getSamples()[state->pos] / (float)UINT16_MAX;
					state->pos_carryover += sfreq / (float)SFREQ;
					float trunc = floor(state->pos_carryover);

					if (channels == 1){
						buffer[i].r = buffer[i].l;
						state->pos += (int)trunc;
					} else if (channels == 2){
						buffer[i].r = soundbuffer.getSamples()[state->pos + 1] / (float)UINT16_MAX;
						state->pos += 2 * (int)trunc;
					}

					state->pos_carryover -= trunc;
				}
			}
		}

		private:
		std::thread loader_thread;
		volatile bool loaded;
		sf::SoundBuffer soundbuffer;
	};

}