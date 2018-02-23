#include "objectUI.h"
#include "numberwireUI.h"
#include "numberinputUI.h"
#include "numberoutputUI.h"

namespace fui {

	// Object
	Object::Object(){
		container = nullptr;
		size = {100, 100};
		bring_to_front = true;
	}
	Container* Object::getContainer(){
		return container;
	}
	void Object::showWireDropList(const std::vector<std::pair<std::function<void()>, std::string>>& items, vec2 pos){
		WireDropList* wdl = new WireDropList(items);
		addChildWindow(wdl, pos - wdl->size * 0.5f);
		wdl->grabFocus();
	}
	void Object::showNumberInputList(NumberWire* wire, vec2 pos){
		std::vector<std::pair<std::function<void()>, std::string>> items;
		for (NumberInput* ni : number_inputs){
			items.push_back(std::make_pair(
				[wire,ni]{
				wire->ConnectHeadTo(ni);
			},
				ni->getCaption()
				));
		}
		showWireDropList(items, pos);
	}
	void Object::showNumberOutputList(NumberWire* wire, vec2 pos){
		std::vector<std::pair<std::function<void()>, std::string>> items;
		for (NumberOutput* no : number_outputs){
			items.push_back(std::make_pair(
				[wire,no]{
				wire->ConnectTailTo(no);
			},
				no->getCaption()
				));
		}
		showWireDropList(items, pos);
	}
	void Object::showSoundInputList(SoundWire* wire, vec2 pos){
		// TODO
	}
	void Object::showSoundOutputList(SoundWire* wire, vec2 pos){
		// TODO
	}
	
} // namespace fui