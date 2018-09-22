#include "FlosionUI.h"

namespace fui {



	ui::Ref<Object> Factory::createObject(const std::string& name) {
		auto& objmap = getObjectMap();
		auto it = objmap.find(name);
		if (it == objmap.end()){
			return nullptr;
		}
		return it->second();
	}

	const std::map<std::string, Factory::ObjectCreator>& Factory::getObjectCreators() {
		return getObjectMap();
	}

	std::map<std::string, Factory::ObjectCreator>& Factory::getObjectMap(){
		static std::map<std::string, Factory::ObjectCreator> theMap;
		return theMap;
	}

} // namespace fui