#include <Flosion/UI/Core/ObjectFactory.hpp>

#include <Flosion/UI/Core/Object.hpp>

#include <cctype>

namespace flui {

    namespace {
        void makeLowerCase(std::string& str) {
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

    std::unique_ptr<Object> Factory::createObject(std::string args){
        const auto& creators = getObjectCreators();
        auto n = args.find_first_not_of(" \t", 0);
        n = args.find_first_of(" \t", n);
        auto name = args.substr(n);
        args = args.substr(args.find_first_not_of(" \t", n));
        makeLowerCase(name);
        if (auto it = creators.find(name); it != creators.end()){
            return it->second(args);
        }
        return nullptr;
    }

	const Factory::ObjectCreatorMap& Factory::getObjectCreators() {
		return getObjectMap();
	}

    void Factory::addCreator(const std::vector<std::string>& names, ObjectCreator creator){
        auto& map = getObjectMap();
        for (const auto& name : names){
            auto n = name;
            makeLowerCase(n);
            map.insert_or_assign(std::move(n), creator);
        }
    }

    void Factory::removeCreator(const std::vector<std::string>& names){
        auto& map = getObjectMap();
        for (const auto& name : names){
            auto n = name;
            makeLowerCase(n);
            auto it = map.find(n);
            assert(it != map.end());
            map.erase(it);
        }
    }

	Factory::ObjectCreatorMap& Factory::getObjectMap(){
		static ObjectCreatorMap theMap;
		return theMap;
	}

} // namespace flui
