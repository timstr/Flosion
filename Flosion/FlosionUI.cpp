#include "FlosionUI.h"
#include "Font.h"
#include "gui/stringhelpers.h"
#include "constantUI.h"
#include "sstreamutil.h"

namespace fui {

	

	// ProcessingObject
	void ProcessingObject::onRightClick(int clicks){
		startDrag();
	}
	bool ProcessingObject::onDropWindow(Window* window){
		if (NumberWire::Head* wirehead = dynamic_cast<NumberWire::Head*>(window)){
			if (number_inputs.size() == 0){
				return false;
			} else if (number_inputs.size() == 1){
				wirehead->wire->connectHeadTo(number_inputs[0]);
				return true;
			} else {
				showNumberInputList(wirehead->wire, localMousePos());
				return true;
			}
		}
		if (NumberWire::Tail* wiretail = dynamic_cast<NumberWire::Tail*>(window)){
			if (number_outputs.size() == 0){
				return false;
			} else if (number_outputs.size() == 1){
				wiretail->wire->connectTailTo(number_outputs[0]);
				return true;
			} else {
				showNumberOutputList(wiretail->wire, localMousePos());
				return true;
			}
		}
		if (SoundWire::Head* wirehead = dynamic_cast<SoundWire::Head*>(window)){
			if (sound_inputs.size() == 0){
				return false;
			} else if (sound_inputs.size() == 1){
				wirehead->wire->connectHeadTo(sound_inputs[0]);
				return true;
			} else {
				showSoundInputList(wirehead->wire, localMousePos());
				return true;
			}
		}
		if (SoundWire::Tail* wiretail = dynamic_cast<SoundWire::Tail*>(window)){
			if (sound_outputs.size() == 0){
				return false;
			} else if (sound_outputs.size() == 1){
				wiretail->wire->connectTailTo(sound_outputs[0]);
				return true;
			} else {
				showSoundOutputList(wiretail->wire, localMousePos());
				return true;
			}
		}
		return false;
	}

	// factory for creating Objects from strings
	Object* Factory::createObject(const std::string& name){
		auto it = getObjectMap().find(name);
		if (it == getObjectMap().end()){
			return nullptr;
		} else {
			return it->second();
		}
	}
	std::map<std::string, std::function<Object*()>>& Factory::getObjectMap(){
		static std::map<std::string, std::function<Object*()>> objmap;
		return objmap;
	}

}