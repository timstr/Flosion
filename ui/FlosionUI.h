#pragma once

#include "objectUI.h"
#include "boxUI.h"
#include "NumberInput.h"
#include "numberoutputUI.h"
#include "numberwireUI.h"
#include "soundinputUI.h"
#include "soundoutputUI.h"
#include "soundwireUI.h"


namespace fui {

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