#pragma once
#include <gui.h>
#include "NumberSource.h"
#include "SoundSource.h"
#include <functional>
#include "Font.h"

#include <iostream> // TODO: remove

namespace fui {

	struct Object;
	struct Container; // TODO: I think this should be called Box
	struct NumberInput;
	struct NumberOutput;
	struct NumberWire;
	struct SoundInput;
	struct SoundOutput;
	struct SoundWire;

	// Object is the base class to all gui components representing sound and
	// numerical processing, and their related components
	struct Object : ui::Window {
		Object();

		Container* getContainer();

		private:

		struct WireDropList : ui::Window {
			WireDropList(const std::vector<std::pair<std::function<void()>, std::string>>& items){
				size.x = 200.0f;
				size.y = 30.0f * items.size();
				int count = 0;
				for (auto& item : items){
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
		
		// TODO: hide invalid inputs/outputs
		void showNumberInputList(NumberWire* wire, vec2 pos);
		void showNumberOutputList(NumberWire* wire, vec2 pos);
		void showSoundInputList(SoundWire* wire, vec2 pos);
		void showSoundOutputList(SoundWire* wire, vec2 pos);

		void showWireDropList(const std::vector<std::pair<std::function<void()>, std::string>>& items, vec2 pos);

		// the FUI container that holds the current object
		// or if this is the master container should this ever be null
		Container* container;
		std::vector<SoundInput*> sound_inputs;
		std::vector<SoundOutput*> sound_outputs;
		std::vector<NumberInput*> number_inputs;
		std::vector<NumberOutput*> number_outputs;

		friend struct Container;
		friend struct SoundInput;
		friend struct SoundOutput;
		friend struct NumberInput;
		friend struct NumberOutput;

		friend struct ProcessingObject;
	};

	// Container is what holds all objects
	// allows convenient clustering of objects and collapsing into a more compact view
	// TODO:
	// Enable selections
	struct Container : Object {

		// add a new object
		void addObject(Object* obj);
		// remove an object, without destroying
		void releaseObject(Object* obj);

		// collapse the container such that its contained objects aren't rendered
		// and it takes up much less screen space
		void collapse();
		// expand the container to show its contents
		void expand();

		void render(sf::RenderWindow& renderwin) override;

		void onLeftClick(int clicks) override;

		private:
		std::vector<Object*> objects;

		struct Menu : ui::Window {
			Menu(Container* _container);
			void beginTyping();
			void onLoseFocus() override;
			void render(sf::RenderWindow& rw) override;
			void refreshList(const std::string& text);
			private:
			struct ListItem : ui::Window {
				ListItem(Menu* _menu, const std::string& _str);
				void onLeftClick(int clicks) override;
				void onKeyDown(sf::Keyboard::Key key) override;
				void render(sf::RenderWindow& rw) override;	
				Menu* menu;
				std::string str;
			};
			std::vector<ListItem*> listitems;
			struct TextField : ui::TextEntry {
				TextField(Menu* _menu);
				void onType(const std::string& text) override;
				void onReturn(const std::string& text) override;
				void onKeyDown(sf::Keyboard::Key key) override;
				Menu* menu;
			}* textentry;
			Container* container;
			void createObject(const std::string& str);
		};

		friend struct Object;
	};

	// MasterContainer is the singleton-intended container that holds everything in the current work area
	struct MasterContainer : Container {
		void render(sf::RenderWindow& renderwin) override;
	};

	// the destination for a numberwire
	// corresponds to the input of a processing object
	struct NumberInput : ui::Window {
		NumberInput(musical::NumberInput* _target, Object* _parent, const std::string& _caption = "", const std::function<void(NumberOutput*)>& _onConnect = {});
		~NumberInput();

		void setWireIn(NumberWire* wire);

		bool onDropWindow(Window* window) override;

		void onHover() override;
		void onHoverWithWindow(Window* win) override;

		void render(sf::RenderWindow& rw) override;

		void onLeftClick(int clicks) override;

		const std::string& getCaption() const;

		const musical::NumberInput* getInput() const;

		private:
		Object* const owner_object;
		musical::NumberInput* const target;
		NumberWire* wire_in;
		ui::Text* caption;
		std::string caption_str;
		double hover_timestamp;
		std::function<void(NumberOutput*)> onConnect;

		friend struct NumberWire;
	};


	// the source of a numberwire
	// corresponds to a number source
	struct NumberOutput : ui::Window {
		NumberOutput(musical::NumberSource* _target, Object* _parent, const std::string& _caption = "", const std::function<void(NumberInput*)>& _onConnect = {});
		~NumberOutput();

		void addWireOut(NumberWire* wire);
		void removeWireOut(NumberWire* wire);

		void render(sf::RenderWindow& rw) override;

		bool onDropWindow(Window* window) override;

		void onHover() override;
		void onHoverWithWindow(Window* win) override;

		void onLeftClick(int clicks) override;

		const std::string& getCaption() const;

		const musical::NumberSource* getSource() const;

		private:
		Object* const owner_object;
		musical::NumberSource* const target;
		std::vector<NumberWire*> wires_out;
		ui::Text* caption;
		std::string caption_str;
		double hover_timestamp;
		std::function<void(NumberInput*)> onConnect;

		friend struct NumberWire;
	};


	// NumberWire connects a NumberInput to a NumberOutput
	struct NumberWire : Object {
		struct Head : ui::Window {
			Head(NumberWire* _wire);
			void onLeftClick(int clicks);
			bool onDropWindow(Window* window) override;
			NumberWire* wire;
		};
		struct Tail : ui::Window {
			Tail(NumberWire* _wire);
			void onLeftClick(int clicks);
			bool onDropWindow(Window* window) override;
			NumberWire* wire;
		};

		NumberWire();
		~NumberWire();

		bool safeToConnect(NumberInput* input);
		bool safeToConnect(NumberOutput* output);
		void ConnectHeadTo(NumberInput* input);
		void ConnectTailTo(NumberOutput* output);

		void render(sf::RenderWindow& rw) override;

		void dragHead();
		void dragTail();

		private:
		NumberInput* dst;
		NumberOutput* src;
		Head* head;
		Tail* tail;

		friend struct NumberInput;
		friend struct NumberOutput;
	};

	
	// the destination for a sound wire
	// corresponds to a sound input of a processing object
	struct SoundInput : ui::Window {
		SoundInput(musical::SoundInput* _target, Object* _parent);
		~SoundInput();

		void setWireIn(SoundWire* wire);

		bool onDropWindow(Window* window) override;

		void render(sf::RenderWindow& rw) override;

		void onLeftClick(int clicks) override;

		private:
		Object* const owner_object;
		musical::SoundInput* const target;
		SoundWire* wire_in;

		friend struct SoundWire;
	};

	// the source of a sound wire
	// corresponds to a sound source
	struct SoundOutput : ui::Window {
		SoundOutput(musical::SoundSource* _target, Object* _parent);
		~SoundOutput();

		void addWireOut(SoundWire* wire);
		void removeWireOut(SoundWire* wire);

		void render(sf::RenderWindow& rw) override;

		bool onDropWindow(Window* window) override;

		void onLeftClick(int clicks) override;

		private:
		Object* const owner_object;
		musical::SoundSource* const target;
		std::vector<SoundWire*> wires_out;

		friend struct SoundWire;
	};


	// SoundWire connects a SoundInput to a SoundOutput
	struct SoundWire : Object {
		struct Head : ui::Window {
			Head(SoundWire* _wire);
			void onLeftClick(int clicks);
			bool onDropWindow(Window* window) override;
			SoundWire* wire;
		};
		struct Tail : ui::Window {
			Tail(SoundWire* _wire);
			void onLeftClick(int clicks);
			bool onDropWindow(Window* window) override;
			SoundWire* wire;
		};

		SoundWire();

		void ConnectHeadTo(SoundInput* input);
		void ConnectTailTo(SoundOutput* output);

		void render(sf::RenderWindow& rw) override;

		void dragHead();
		void dragTail();

		private:
		SoundInput* dst;
		SoundOutput* src;
		Head* head;
		Tail* tail;

		friend struct SoundInput;
		friend struct SoundOutput;
	};


	// ProcessingObject is the base class to all further-defined sound and number processing objects
	// defines functionality for adding/removing sound/number inputs/outputs
	// each sound/number input/output must be added when it is desired to be used in the current project
	// either in the constructor or dynamically
	// 
	struct ProcessingObject : Object {

		void onRightClick(int clicks) override;

		bool onDropWindow(Window* window) override;
	};


	// factory for creating Objects from strings
	struct Factory {
		template<class ObjectType>
		struct RegisterObject {
			RegisterObject(const std::vector<std::string>& names){
				for (const std::string& name : names){
					getObjectMap()[name] = instantiate<ObjectType>;
				}
			}
		};

		static Object* createObject(const std::string& name);

		static std::map<std::string, std::function<Object*()>>& getObjectMap();

		private:
		Factory() = delete;

		template<class ObjectType>
		static Object* instantiate(){
			static_assert(std::is_base_of<Object, ObjectType>::value, "The provided object type must derive from Object");
			return new ObjectType();
		}
	};
}

// macro for compact registration of Object derived classes
#define fuiRegisterObject(type, ...) \
namespace { \
	::fui::Factory::RegisterObject<type> register_object_##type({##__VA_ARGS__}); \
}