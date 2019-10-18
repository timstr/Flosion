#include <Flosion/UI/Core/ObjectFactory.hpp>

#include <Flosion/UI/Core/Object.hpp>

#include <cctype>

namespace flui {

    namespace detail {
        void makeLowerCase(ui::String& str) {
			std::transform(
			    str.begin(),
			    str.end(),
			    str.begin(),
			    [](uint32_t c){
                    if (c > std::numeric_limits<unsigned char>::max()){
                        return c;
                    }
                    return static_cast<uint32_t>(std::tolower(static_cast<int>(c)));
                }
		    );
		}
    }

    std::unique_ptr<Object> Factory::createObject(ui::String name){
        const auto& creators = getObjectCreators();
        detail::makeLowerCase(name);
        if (auto it = creators.find(name); it != creators.end()){
            return it->second();
        }
        return nullptr;
    }

	const std::map<std::string, Factory::ObjectCreator>& Factory::getObjectCreators() {
		return getObjectMap();
	}

    void Factory::addCreator(const std::vector<ui::String>& names, ObjectCreator creator){
        auto& map = getObjectMap();
        for (const auto& name : names){
            auto n = name;
            detail::makeLowerCase(n);
            map.insert_or_assign(std::move(n), creator);
        }
    }

    void Factory::removeCreator(const std::vector<ui::String>& names){
        auto& map = getObjectMap();
        for (const auto& name : names){
            auto n = name;
            detail::makeLowerCase(n);
            auto it = map.find(n);
            assert(it != map.end());
            map.erase(it);
        }
    }

	std::map<std::string, Factory::ObjectCreator>& Factory::getObjectMap(){
		static std::map<std::string, Factory::ObjectCreator> theMap;
		return theMap;
	}

} // namespace flui
