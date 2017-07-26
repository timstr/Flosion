#pragma once

#include "SoundSource.h"

namespace musical {

	SoundSource::~SoundSource(){
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->setSource(nullptr);
		}
		inputs.clear();

		// TODO: fix this up
		// Note: When this destructor is called, the destructor for SoundSourceTemplate has already been called
		// and this object is effectively only of type SoundSource
		// thus the calls from SoundInput::setSource to SoundSource::removeState are UB?
		// maybe?
		// perhaps do this work in SoundSourceTemplate

	}
	void SoundSource::addDstInput(SoundInput* input){
		for (int i = 0; i < dsts.size(); i++){
			if (dsts[i] == input){
				throw std::runtime_error("The destination SoundInput has already been added");
			}
		}
		dsts.push_back(input);
	}
	void SoundSource::removeDstInput(SoundInput* input){
		for (int i = 0; i < dsts.size(); i++){
			if (dsts[i] == input){
				dsts.erase(dsts.begin() + i);
				return;
			}
		}
		throw std::runtime_error("The destination SoundInput could not be found");
	}
	bool SoundSource::findSource(Stateful* src){
		if (src == this){
			return true;
		}

		for (SoundInput* input : inputs){
			if (input->findSource(src)){
				return true;
			}
		}

		return false;
	}
	void SoundSource::addSoundInput(SoundInput* input){
		for (int i = 0; i < inputs.size(); i++){
			if (inputs[i] == input){
				throw std::runtime_error("The SoundInput is already registered");
			}
		}

		inputs.push_back(input);
	}
	void SoundSource::removeSoundInput(SoundInput* input){
			for (int i = 0; i < inputs.size(); i++){
				if (inputs[i] == input){
					inputs.erase(inputs.begin() + i);
					return;
				}
			}
			throw std::runtime_error("The SoundInput could not be found");
		}

}