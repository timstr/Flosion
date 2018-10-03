#pragma once

#include "objectUI.h"
#include "boxUI.h"
#include "numberwireUI.h"
#include "soundwireUI.h"
#include <cctype>


namespace fui {

	// The entry point of it all
	struct FlosionUI : ui::FreeElement {
		FlosionUI();

	private:

		void moveWithMouse();

		void render(sf::RenderWindow& rw) override;

		ui::Ref<MainBox> box;

		// time that mouse first moved to border
		// when this passes a threshold, the main box begins moving with the mouse
		float mouse_timestamp;
	};

	// factory for creating Objects from strings
	// which have been pre-registered
	struct Factory {
		using ObjectCreator = std::function<ui::Ref<Object>()>;

		static ui::Ref<Object> createObject(const std::string& name);

		static const std::map<std::string, ObjectCreator>& getObjectCreators();

		// RegisterObject for registering an Object type with the factory under a set of names
		// RegisterObject is intended to be used as a static object,
		// one with each Object sub-class definition
		template<typename ObjectType>
		struct RegisterObject {
			RegisterObject(std::vector<std::string> _names) :
				names(Factory::makeLowerCase(_names)) {

				static_assert(std::is_base_of<Object, ObjectType>::value, "The provided object type must derive from Object");

				auto& objectMap = Factory::getObjectMap();
				for (const std::string& name : names) {
					auto it = objectMap.find(name);
					if (it != objectMap.end()){
						throw std::runtime_error("The object map has conflicting names");
					}
					objectMap[name] = [](){ return ui::create<ObjectType>(); };
				}
			}
			~RegisterObject() {
				auto& objectMap = Factory::getObjectMap();
				for (const std::string& name : names){
					auto it = objectMap.find(name);
					if (it != objectMap.end()){
						objectMap.erase(it);
					}
				}
			}

		private:

			const std::vector<std::string> names;
		};


	private:

		Factory() = delete;

		static std::vector<std::string>& makeLowerCase(std::vector<std::string>& strings) {
			for (auto& str : strings){
				std::transform(
					str.begin(),
					str.end(),
					str.begin(),
					[](unsigned char c){ return std::tolower(c); }
				);
			}
			return strings;
		}

		static std::map<std::string, ObjectCreator>& getObjectMap();
	};

	// convenience macro for registering a type. Creates an anonymouse namespace
	// which contains a uniquely named RegisterObject of the provided type.
	// names are converted to lower case.
	// Usage:
	// struct struct CrazyCoolObject : fui::Object { ... };
	// RegisterObject(CrazyCoolObject, "CrazyCoolObject", "crazyobject", "coolObject")
	#define RegisterFactoryObject(objectType, ...) \
	namespace { \
		::fui::Factory::RegisterObject<objectType> factoryRegisterObject_##objectType { std::vector<std::string> { __VA_ARGS__ } }; \
	}
}