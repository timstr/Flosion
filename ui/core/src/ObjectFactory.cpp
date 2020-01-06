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
			    [](int c){
                    return static_cast<char>(std::tolower(c));
                }
		    );
		}
    }

    std::unique_ptr<Object> Factory::createObject(std::string name, const std::vector<std::string>& args){
        const auto& creators = getObjectMap();
        makeLowerCase(name);
        if (auto it = creators.find(name); it != creators.end()){
            auto o = it->second(args);
            o->initialize();
            return o;
        }
        return nullptr;
    }

    void Factory::addCreator(const std::vector<std::string>& names, ObjectCreator creator){
        auto& map = getObjectMap();
        auto& objNames = getObjectNameVector();
        for (const auto& name : names){
            auto n = name;
            makeLowerCase(n);
            auto it = map.find(n);
            if (it != map.end()){
                throw std::runtime_error("An object with the same name is already registered");
            }
            map.emplace(n, creator);
            assert(count(begin(objNames), end(objNames), n) == 0);
            objNames.push_back(std::move(n));
        }
    }

    void Factory::removeCreator(const std::vector<std::string>& names){
        auto& map = getObjectMap();
        auto& objNames = getObjectNameVector();
        for (const auto& name : names){
            auto n = name;
            makeLowerCase(n);
            {
                auto it = map.find(n);
                assert(it != map.end());
                map.erase(it);
            }
            {
                auto it = find(begin(objNames), end(objNames), n);
                assert(it != end(objNames));
                objNames.erase(it);
            }
        }
    }

    const std::vector<std::string>& Factory::getObjectNames() noexcept {
        return getObjectNameVector();
    }

	std::map<std::string, Factory::ObjectCreator>& Factory::getObjectMap(){
		static std::map<std::string, Factory::ObjectCreator> theMap;
		return theMap;
	}

    std::vector<std::string>& Factory::getObjectNameVector(){
        static std::vector<std::string> theNames;
        return theNames;
    }

} // namespace flui
