#include "FlosionUI.h"
#include "FlosionUI.h"

namespace fui {

	FlosionUI::FlosionUI() : mouse_timestamp(-10.0f) {
		box = add<MainBox>();
		setBackgroundColor(sf::Color(0x808080FF));
	}

	void FlosionUI::moveWithMouse()	{
		const float threshold_wait = 0.5f;
		const float border = 100.0f;

		vec2 mouse = localMousePos();
		vec2 delta = {0, 0};
		bool bordermouse = false;
		if (mouse.x >= 0.0f && mouse.x < border){
			delta.x = border - mouse.x;
			bordermouse = true;
		} else if (mouse.x < width() && mouse.x > width() - border){
			delta.x = width() - border - mouse.x;
			bordermouse = true;
		}
		if (mouse.y >= 0.0f && mouse.y < border){
			delta.y = border - mouse.y;
			bordermouse = true;
		} else if (mouse.y < height() && mouse.y > height() - border){
			delta.y = height() - border - mouse.y;
			bordermouse = true;
		}
		if (bordermouse){
			const float now = ui::getProgramTime();
			if (mouse_timestamp < 0.0f){
				mouse_timestamp = now;
			} else if (now >= mouse_timestamp + threshold_wait){
				delta *= 0.1f;
				delta.x = roundf(delta.x);
				delta.y = roundf(delta.y);
				box->setPos(box->pos() + delta);
			}
		} else {
			mouse_timestamp = -10.0f;
		}
	}

	void FlosionUI::render(sf::RenderWindow& rw) {
		setSize(ui::getScreenSize(), true);
		moveWithMouse();
	}

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