#include <Flosion/UI/Core/Object.hpp>

#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {
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

	void Object::onDrag() {
		updateWires();
	}

	void Object::updateWires() {
		for (const auto& si : sound_inputs){
			if (auto x = si.lock()){
				x->updateWire();
			}
		}
		for (const auto& so : sound_outputs){
			if (auto x = so.lock()){
				x->updateWires();
			}
		}
		for (const auto& ni : number_inputs){
			if (auto x = ni.lock()){
				x->updateWire();
			}
		}
		for (const auto& no : number_outputs){
			if (auto x = no.lock()){
				x->updateWires();
			}
		}
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

	void Object::addSoundInput(ui::Ref<SoundInput> si) {
		sound_inputs.push_back(si);
	}

	void Object::removeSoundInput(SoundInput& si) {
		si.m_parentobject.reset();
		auto& vec = sound_inputs;
		auto shouldRemove = [si](const auto& x){
			auto xl = x.lock();
			return !xl || (xl.get() == &si);
		};
		vec.erase(std::remove_if(vec.begin(), vec.end(), shouldRemove), vec.end());
	}

	void Object::addSoundOutput(ui::Ref<SoundOutput> so) {
		sound_outputs.push_back(so);
	}

	void Object::removeSoundOutput(SoundOutput& so) {
		so.m_parentobject.reset();
		auto& vec = sound_outputs;
		auto shouldRemove = [so](const auto& x){
			auto xl = x.lock();
			return !xl || (xl.get() == &so);
		};
		vec.erase(std::remove_if(vec.begin(), vec.end(), shouldRemove), vec.end());
	}

	void Object::addNumberInput(ui::Ref<NumberInput> ni) {
		number_inputs.push_back(ni);
	}

	void Object::removeNumberInput(NumberInput& ni) {
		ni.m_parentobject.reset();
		auto& vec = number_inputs;
		auto shouldRemove = [ni](const auto& x){
			auto xl = x.lock();
			return !xl || (xl.get() == &ni);
		};
		vec.erase(std::remove_if(vec.begin(), vec.end(), shouldRemove), vec.end());
	}

	void Object::addNumberOutput(ui::Ref<NumberOutput> no) {
		number_outputs.push_back(no);
	}

	void Object::removeNumberOutput(NumberOutput& no) {
		no.m_parentobject.reset();
		auto& vec = number_outputs;
		auto shouldRemove = [no](const auto& x){
			auto xl = x.lock();
			return !xl || (xl.get() == &no);
		};
		vec.erase(std::remove_if(vec.begin(), vec.end(), shouldRemove), vec.end());
	}

} // namespace flui