#pragma once

#include <GUI/GUI.hpp>

namespace flui {

    class Object;

    // TODO: allow arguments to be passed and parsed
    // Examples:
    // constant 6
    // slider 0 10
    // random uniform 0 1
    // random normal 10 1
    // expression x+(1/y)

	// factory for creating Objects from strings
	// which have been pre-registered
	class Factory {
    public:
		using ObjectCreator = std::function<std::unique_ptr<Object>(const std::string&)>;

        using ObjectCreatorMap = std::map<std::string, ObjectCreator>;

		static std::unique_ptr<Object> createObject(std::string args);

		static const ObjectCreatorMap& getObjectCreators();

        static void addCreator(const std::vector<std::string>& names, ObjectCreator creator);

        static void removeCreator(const std::vector<std::string>& names);

    public:
		// RegisterObject for registering an Object type with the factory under a set of names
		// RegisterObject is intended to be used as a static object,
		// one with each Object sub-class definition
		template<typename ObjectType>
		class Registrator {
        public:
			Registrator(std::vector<std::string> names, ObjectCreator);
			~Registrator();

        private:
			const std::vector<std::string> m_names;
		};


	private:
		Factory() = delete;

		static ObjectCreatorMap& getObjectMap();
	};

    // TODO: move these to a .tpp file
    template<typename ObjectType>
    inline Factory::Registrator<ObjectType>::Registrator(std::vector<std::string> names, ObjectCreator oc)
        : m_names(std::move(names)) {

		static_assert(std::is_base_of<Object, ObjectType>::value, "The provided object type must derive from Object");
        static_assert(std::is_default_constructible_v<ObjectType>, "The provided object type must be default constructible");

        addCreator(m_names, std::move(oc));
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
	#define RegisterFactoryObject(ObjectType, ...) \
	    namespace FlosionUIFactoryImpl { \
		    ::flui::Factory::Registrator<ObjectType> s_registratorFor_##ObjectType { \
                std::vector<std::string> { __VA_ARGS__ }, \
                [](const std::string&){ return std::make_unique<ObjectType>(); } \
            }; \
	    }

    #define RegisterFactoryObjectEx(ObjectType, Name, CreatorFunction) \
        namespace FlosionUIFactoryImpl { \
            ::flui::Factory::Registrator<ObjectType> s_registratorFor_##ObjectType { \
                std::vector<std::string> { Name }, \
                CreatorFunction \
            }; \
        }

}