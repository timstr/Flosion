#include "objectUI.h"

namespace fui {
	Object::Object() {
		setBackgroundColor(sf::Color(0xBB7700FF));
		setBorderRadius(10);
		setPadding(5);
	}

	bool Object::onLeftClick(int clicks) {
		startDrag();
		bringToFront();
		return true;
	}

	void Object::onLeftRelease() {
		stopDrag();
	}

	ui::Ref<Box> Object::getParentBox() {
		return m_parentbox.lock();
	}

	void Object::showNumberInputList(ui::WeakRef<NumberWire> wire, vec2 pos) {
		// TODO
	}

	void Object::showNumberOutputList(ui::WeakRef<NumberWire> wire, vec2 pos) {
		// TODO
	}

	void Object::showSoundInputList(ui::WeakRef<SoundWire> wire, vec2 pos) {
		// TODO
	}

	void Object::showSoundOutputList(ui::WeakRef<SoundWire> wire, vec2 pos) {
		// TODO
	}
	
	void Object::showWireDropList(const std::vector<std::pair<std::string, std::function<void()>>>& items, vec2 pos) {
		// TODO
	}

} // namespace fui