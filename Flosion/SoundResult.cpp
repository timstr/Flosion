#pragma once

#include "SoundResult.h"

namespace musical {

	SoundResult::SoundResult() : input(nullptr) {

	}
	SoundResult::~SoundResult(){

	}

	void SoundResult::getNextChunk(Sample* buffer){
		input.getNextChunk(buffer, nullptr);
	}

	void SoundResult::reset(){
		input.resetState(nullptr);
	}

	void SoundResult::setSource(SoundSource* source){
		input.setSource(source);
	}

}