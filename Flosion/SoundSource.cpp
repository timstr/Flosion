#pragma once

#include "SoundSource.h"

namespace musical {

	// SoundInput base

	SoundInput::SoundInput(SoundSource* _parent){
		source = nullptr;
		if (_parent){
			_parent->addSoundInput(this);
		}
		parent = _parent;
	}
	SoundInput::~SoundInput(){
		if (parent){
			parent->removeSoundInput(this);
		}
	}
	SoundSource* SoundInput::getSource() const {
		return source;
	}
	bool SoundInput::hasSource() const {
		return source != nullptr;
	}
	bool SoundInput::findSource(Stateful* src) const {
		if (this == src){
			return true;
		}
		if (source){
			return source->findSource(src);
		}
		return false;
	}


	// SingleInput

	SingleInput::SingleInput(SoundSource* parent) : SoundInput(parent) {

	}
	SingleInput::~SingleInput(){
		setSource(nullptr);
	}
	void SingleInput::getNextChunk(Sample* buffer, State* state){
		for (int i = 0; i < CHUNK_SIZE; i++){
			buffer[i] = {0, 0};
		}
		if (source){
			source->getNextChunk(buffer, state, this);
		}
	}
	void SingleInput::addState(State* parent_state){
		if (source){
			source->addState(parent_state, this);
		}
	}
	void SingleInput::removeState(State* parent_state){
		if (source){
			source->removeState(parent_state, this);
		}
	}
	void SingleInput::resetState(State* parent_state){
		if (source){
			source->resetState(parent_state, this);
		}
	}
	void SingleInput::setSource(SoundSource* _source){
		if (source){
			source->removeDstInput(this);
			if (parent){
				parent->removeAllStatesFrom(this);
			} else {
				source->removeState(nullptr, this);
			}
		}

		source = _source;

		if (source){
			if (parent){
				parent->addAllStatesTo(this);
			} else {
				source->addState(nullptr, this);
			}
			source->addDstInput(this);
		}
	}

	// SoundSource

	SoundSource::~SoundSource(){
		for (int i = 0; i < inputs.size(); i++){
			inputs[i]->setSource(nullptr);
		}
		inputs.clear();
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
	bool SoundSource::findSource(Stateful* src) const {
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