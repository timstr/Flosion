#pragma once
#include "gui.h"
#include "NumberSource.h"
#include "SoundSource.h"
#include <functional>

#include <iostream> // TODO: remove

namespace fui {

	struct Object;
	struct Container;
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
		// the FUI container that holds the current object
		// on if this is the master container should this ever be null
		Container* container;

		friend struct Container;
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
			private:
			ui::TextEntry* textentry;
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
		NumberInput(musical::NumberInput* _target, Object* _parent, const std::string& _caption = "", sf::Vector2f _pos = {0,0});

		void setWireIn(NumberWire* wire);

		void onDropDragWindow(Window* window) override;

		void onHover() override;
		void onHoverWithDrag(Window* win) override;

		void render(sf::RenderWindow& rw) override;

		void onLeftClick(int clicks) override;

		private:
		Object* parent;
		musical::NumberInput* target;
		NumberWire* wire_in;
		ui::Text* caption;
		double hover_timestamp;

		friend struct NumberWire;
	};


	// the source of a numberwire
	// corresponds to a number source
	struct NumberOutput : ui::Window {
		NumberOutput(musical::NumberSource* _target, Object* _parent, const std::string& _caption = "", sf::Vector2f _pos = {0,0});

		void addWireOut(NumberWire* wire);
		void removeWireOut(NumberWire* wire);

		void render(sf::RenderWindow& rw) override;

		void onDropDragWindow(Window* window) override;

		void onHover() override;
		void onHoverWithDrag(Window* win) override;

		void onLeftClick(int clicks) override;

		private:
		Object* parent;
		musical::NumberSource* target;
		std::vector<NumberWire*> wires_out;
		ui::Text* caption;
		double hover_timestamp;

		friend struct NumberWire;
	};


	// NumberWire connects a NumberInput to a NumberOutput
	struct NumberWire : Object {
		struct Head : ui::Window {
			Head(NumberWire* _wire);
			void onLeftClick(int clicks);
			NumberWire* wire;
		};
		struct Tail : ui::Window {
			Tail(NumberWire* _wire);
			void onLeftClick(int clicks);
			NumberWire* wire;
		};

		NumberWire();

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
		SoundInput(musical::SoundInput* _target, Object* _parent, sf::Vector2f _pos = {0,0});

		void setWireIn(SoundWire* wire);

		void onDropDragWindow(Window* window) override;

		void render(sf::RenderWindow& rw) override;

		void onLeftClick(int clicks) override;

		private:
		Object* parent;
		musical::SoundInput* target;
		SoundWire* wire_in;

		friend struct SoundWire;
	};

	// the source of a sound wire
	// corresponds to a sound source
	struct SoundOutput : ui::Window {
		SoundOutput(musical::SoundSource* _target, Object* _parent, sf::Vector2f _pos = {0,0});

		void addWireOut(SoundWire* wire);
		void removeWireOut(SoundWire* wire);

		void render(sf::RenderWindow& rw) override;

		void onDropDragWindow(Window* window) override;

		void onLeftClick(int clicks) override;

		private:
		Object* parent;
		musical::SoundSource* target;
		std::vector<SoundWire*> wires_out;

		friend struct SoundWire;
	};


	// SoundWire connects a SoundInput to a SoundOutput
	struct SoundWire : Object {
		struct Head : ui::Window {
			Head(SoundWire* _wire);
			void onLeftClick(int clicks);
			SoundWire* wire;
		};
		struct Tail : ui::Window {
			Tail(SoundWire* _wire);
			void onLeftClick(int clicks);
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

		void onDropDragWindow(Window* window) override;

		void addSoundInput(SoundInput* si);
		void removeSoundInput(SoundInput* si);

		void addSoundOutput(SoundOutput* so);
		void removeSoundOutput(SoundOutput* so);

		void addNumberInput(NumberInput* ni);
		void removeNumberInput(NumberInput* ni);

		void addNumberOutput(NumberOutput* no);
		void removeNumberOutput(NumberOutput* no);

		private:
		std::vector<SoundInput*> sound_inputs;
		std::vector<SoundOutput*> sound_outputs;
		std::vector<NumberInput*> number_inputs;
		std::vector<NumberOutput*> number_outputs;
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

		private:
		Factory() = delete;

		static std::map<std::string, std::function<Object*()>>& getObjectMap();

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