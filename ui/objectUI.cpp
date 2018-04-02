#include "objectUI.h"
#include "numberwireUI.h"
#include "numberinputUI.h"
#include "numberoutputUI.h"
#include "soundwireUI.h"
#include "soundinputUI.h"
#include "soundoutputUI.h"

namespace fui {

	// Object
	Object::Object(){
		box = nullptr;
		size = {100, 100};
		bring_to_front = true;
	}
	Box* Object::getBox(){
		return box;
	}

	struct WireDropList : ui::Window {
		WireDropList(const std::vector<std::pair<std::function<void()>, std::string>>& items) {
			size.x = 200.0f;
			size.y = 30.0f * items.size();
			int count = 0;
			for (auto& item : items) {
				addChildWindow(new ListItem(this, item.first, item.second), vec2(0, count * 30.0f));
				count++;
			}
		}

		void onLoseFocus() override {
			close();
		}

		void render(sf::RenderWindow& rw) override {
			renderChildWindows(rw);
		}

		struct ListItem : ui::Window {
			ListItem(WireDropList* _parent, const std::function<void()>& fn, const std::string& str) : callback(fn), parent(_parent) {
				clipping = true;
				size = vec2(200.0f, 30.0f);
				addChildWindow(new ui::Text(str, getFont(), sf::Color(0xFFFFFFFF), 20), insideLeft(this, 5.0f), insideTop(this, 0.0f));
			}

			void onLeftClick(int clicks) override {
				callback();
				parent->close();
			}

			void render(sf::RenderWindow& rw) override {
				sf::RectangleShape rect;
				rect.setPosition(vec2(1.0f, 1.0f));
				rect.setSize(size - vec2(2.0f, 2.0f));
				rect.setFillColor(sf::Color(0x000000C0));
				rect.setOutlineColor(sf::Color(0x80808080));
				rect.setOutlineThickness(1.0f);
				rw.draw(rect);
				renderChildWindows(rw);
			}

			WireDropList* const parent;
			const std::function<void()> callback;
		};
	};

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
					wire->connectHeadTo(ni);
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
					wire->connectTailTo(no);
				},
				no->getCaption()
			));
		}
		showWireDropList(items, pos);
	}
	void Object::showSoundInputList(SoundWire* wire, vec2 pos){
		std::vector<std::pair<std::function<void()>, std::string>> items;
		for (SoundInput* si : sound_inputs){
			items.push_back(std::make_pair(
				[wire,si]{
					wire->connectHeadTo(si);
				},
				si->getCaption()
			));
		}
		showWireDropList(items, pos);
	}
	void Object::showSoundOutputList(SoundWire* wire, vec2 pos){
		std::vector<std::pair<std::function<void()>, std::string>> items;
		for (SoundOutput* so : sound_outputs){
			items.push_back(std::make_pair(
				[wire,so]{
					wire->connectTailTo(so);
				},
				so->getCaption()
			));
		}
		showWireDropList(items, pos);
	}
	
} // namespace fui