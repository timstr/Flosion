#pragma once

#include "guiformscontrol.h"
#include <map>
#include <memory>
#include <typeinfo>
#include <typeindex>

namespace ui {
	namespace forms {

		struct Property {
			Property(const std::type_index& _typeindex) : typeindex(_typeindex) {

			}
			virtual ~Property(){

			}

			template<typename T>
			T& get(){
				if (typeindex == std::type_index(typeid(T))){
					return *(T*)getValue();
				} else {
					throw std::bad_cast();
				}
			}

			template<typename T>
			void operator=(const T& t){
				get() = t;
			}

			template<typename T>
			operator const T&(){
				return get();
			}

			virtual Control* makeControl() = 0;

			private:
			const std::type_index typeindex;

			virtual void* getValue() = 0;
		};

		template<typename Type>
		struct TypeProperty : Property {
			TypeProperty(const Type& _val = {})
				: Property(std::type_index(typeid(Type))),
				value(_val) {

			}

			Type value;

			private:

			void* getValue() override {
				return &value;
			}
		};

		struct Model {

			struct Proxy;

			Proxy& operator[](const std::string& name){
				return properties[name];
			}

			struct Proxy {
				template<typename PropertyType>
				void operator=(const PropertyType& p){
					static_assert(std::is_base_of<Property, PropertyType>::value, "The provided type must derive from Property");
					property = std::make_shared<PropertyType>(p);
				}

				operator Property*(){
					return property.get();
				}

				Property* operator->(){
					return property.get();
				}

				template<typename T>
				operator const T&(){
					return property->get<T>();
				}

				private:
				std::shared_ptr<Property> property;
			};

			private:
			std::map<std::string, Proxy> properties;
			friend struct Form;
		};

	}
}