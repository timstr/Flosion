#pragma once

#include "ObjectFactory.h"
#include "phasevocoder.h"
#include <GUI/Helpers.hpp>

namespace fui {

	struct PhaseVocoderObject : Object {
		PhaseVocoderObject(){
			auto self = this->thisAs<Object>();
			add<SoundInput>(self, phasevocoder.input, "Sound Input");
			add<SoundOutput>(self, phasevocoder, "Sound Output");
			add<NumberInput>(self, phasevocoder.timespeed, "Time Speed");
			write("Phase Vocoder", getFont());
			add<ui::PullDownMenu<uint32_t>>(
				std::vector<uint32_t>{256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 1<<16, 1<<17},
				getFont(),
				[this](const unsigned& val){
					phasevocoder.setWindowSize(val);
				}
			);
		}

		private:

		flo::PhaseVocoder phasevocoder;
	};
	RegisterFactoryObject(PhaseVocoderObject, "phase vocoder");

}