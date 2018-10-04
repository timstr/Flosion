#pragma once

#include "ObjectFactory.h"
#include "phasevocoder.h"
#include <GUI/Helpers.hpp>

namespace fui {

	struct PhaseVocoderObject : Object {
		PhaseVocoderObject(){
			auto self = thisAs<Object>();
			add<SoundInput>(self, phasevocoder.input, "Sound Input");
			add<SoundOutput>(self, phasevocoder, "Sound Output");
			add<NumberInput>(self, phasevocoder.timespeed, "Time Speed");
			write("Phase Vocoder", getFont());
			add<ui::PullDownMenu<unsigned>>(
				std::vector<unsigned>{256, 512, 1024, 2048, 4096, 8192},
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