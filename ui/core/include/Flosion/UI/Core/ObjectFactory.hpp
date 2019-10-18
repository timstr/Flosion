#pragma once

#include <GUI/GUI.hpp>

namespace flui {

    class Object;

	// factory for creating Objects from strings
	// which have been pre-registered
	class Factory {
    public:
		using ObjectCreator = std::function<std::unique_ptr<Object>()>;

		static std::unique_ptr<Object> createObject(ui::String name);

		static const std::map<std::string, ObjectCreator>& getObjectCreators();

        static void addCreator(const std::vector<ui::String>& names, ObjectCreator creator);

        static void removeCreator(const std::vector<ui::String>& names);

    private:
		// RegisterObject for registering an Object type with the factory under a set of names
		// RegisterObject is intended to be used as a static object,
		// one with each Object sub-class definition
		template<typename ObjectType>
		class Registrator {
        public:
			Registrator(std::vector<std::string> names);
			~Registrator();

        private:
			const std::vector<ui::String> m_names;
		};


	private:
		Factory() = delete;

		static std::map<std::string, ObjectCreator>& getObjectMap();
	};

    // TODO: move these to a .tpp file
    template<typename ObjectType>
    inline Factory::Registrator<ObjectType>::Registrator(std::vector<std::string> names)
        : m_names(Factory::makeLowerCase(std::move(names))) {

		static_assert(std::is_base_of<Object, ObjectType>::value, "The provided object type must derive from Object");
        static_assert(std::is_default_constructible<ObjectType>, "The provided object type must be default constructible");

        addCreator(m_names, [](){ return std::make_unique<ObjectType>(); });
    }

    template<typename ObjectType>
    inline Factory::Registrator<ObjectType>::~Registrator(){
        removeCreator(m_names);
    }

	// convenience macro for registering a type. Creates an anonymouse namespace
	// which contains a uniquely named RegisterObject of the provided type.
	// names are converted to lower case.
	// Usage:
	// struct CrazyCoolObject : flui::Object { /* ... */ };
	// RegisterObject(CrazyCoolObject, "CrazyCoolObject", "crazyobject", "coolObject")
	#ifndef RegisterFactoryObject
	#define RegisterFactoryObject(objectType, ...) \
	namespace FlosionUIFactoryImpl { \
		::flui::Factory::RegisterObject<objectType> s_registratorFor_##objectType { std::vector<std::string> { __VA_ARGS__ } }; \
	}
	#endif // RegisterFactoryObject

}