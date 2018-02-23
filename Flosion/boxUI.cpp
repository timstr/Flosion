#include "boxUI.h"
#include <sstream>
#include "constantUI.h"
#include "sstreamutil.h"

namespace fui {

	// Container
	void Container::addObject(Object* obj){
		addChildWindow(obj);
		objects.push_back(obj);
		obj->container = this;
	}
	void Container::releaseObject(Object* obj){
		releaseChildWindow(obj);
		for (int i = 0; i < objects.size(); i++){
			if (objects[i] == obj){
				objects.erase(objects.begin() + i);
				break;
			}
		}
		obj->container = nullptr;
	}
	void Container::collapse(){
		// TODO
	}
	void Container::expand(){
		// TODO
	}
	void Container::render(sf::RenderWindow& rw){
		sf::Vertex vertices[] = {
			sf::Vertex({0, 0}, sf::Color(0x404040FF)),
			sf::Vertex({size.x, 0}, sf::Color(0x404040FF)),
			sf::Vertex({size.x, size.y}, sf::Color(0x202020FF)),
			sf::Vertex({0, size.y}, sf::Color(0x202020FF)),
		};
		rw.draw(vertices, 4, sf::PrimitiveType::Quads);
		renderChildWindows(rw);
	}
	void Container::onLeftClick(int clicks){
		if (clicks == 2){
			Menu* menu = new Menu(this);
			addChildWindow(menu);
			menu->pos = localMousePos() - menu->size * 0.5f;
			menu->beginTyping();
		}
	}
	Container::Menu::Menu(Container* _container){
		container = _container;
		size = {100, 20};
		addChildWindow(textentry = new TextField(this));
	}
	void Container::Menu::beginTyping(){
		textentry->grabFocus();
	}
	void Container::Menu::onLoseFocus(){
		close();
	}
	Object* Container::Menu::createObject(const std::string& str){
		Object* obj;

		obj = createConstant(str);
		if (obj){
			return obj;
		}

		obj = Factory::createObject(str);
		if (!obj && listitems.size() > 0){
			return Factory::createObject(listitems.front()->str);
		} else {
			return obj;
		}
	}
	Object* Container::Menu::createConstant(const std::string& str){
		std::stringstream stream;

		float val1, val2;
		std::string name;

		stream.str(str);

		if (streamRead(stream, val1)){
			if (streamRead(stream, val2)){
				if (!stream.eof()){
					name = stream.str();
					return new SliderObject(val1, val2); // TODO: pass name along
				} else {
					return new SliderObject(val1, val2);
				}
			} else {
				if (!stream.eof()){
					name = stream.str();
					return new ConstantObject(val1); // TODO: pass name along
				} else {
					return new ConstantObject(val1);
				}
			}
		} else {
			return nullptr;
		}
	}
	void Container::Menu::addObject(Object* obj){
		if (obj){
			obj->pos = vec2(sf::Vector2i(pos + size * 0.5f - obj->size * 0.5f));
			container->addObject(obj);
		}
	}
	void Container::Menu::render(sf::RenderWindow& rw){
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(0x404040FF));
		rect.setSize(size);
		rect.setOutlineColor(sf::Color(0x808080FF));
		rect.setOutlineThickness(1.0f);
		rw.draw(rect);
		renderChildWindows(rw);
	}
	void Container::Menu::refreshList(const std::string& text){
		for (ListItem* li : listitems){
			li->close();
		}
		listitems.clear();
		auto& objmap = Factory::getObjectMap();
		float y = size.y;
		int count = 0;
		const int max_items = 10;
		for (auto it = objmap.begin(); it != objmap.end() && count < max_items; it++){
			if (it->first.find(text) == 0){
				ListItem* li = new ListItem(this, it->first);
				listitems.push_back(li);
				addChildWindow(li);
				li->pos.y = y;
				y += li->size.y;
				count++;
			}
		}
	}
	Container::Menu::TextField::TextField(Menu* _menu) : TextEntry("", getFont(), 15, sf::Color(0xFFFFFFFF), sf::Color(0x0)){
		menu = _menu;
	}
	void Container::Menu::TextField::onType(std::string text){
		menu->refreshList(text);
	}
	void Container::Menu::TextField::onReturn(std::string text){
		menu->addObject(menu->createObject(text));
		menu->close();
	}
	void Container::Menu::TextField::onKeyDown(sf::Keyboard::Key key){
		if (key == sf::Keyboard::Down){
			focusToNextWindow();
		} else if (key == sf::Keyboard::Up){
			focusToPreviousWindow();
		}
	}
	Container::Menu::ListItem::ListItem(Menu* _menu, const std::string& _str){
		menu = _menu;
		str = _str;
		size = {100, 20};
		addChildWindow(new ui::Text(str, getFont(), sf::Color(0xFFFFFFFF), 15));
	}
	void Container::Menu::ListItem::onLeftClick(int clicks){
		menu->addObject(menu->createObject(str));
		menu->close();
	}
	void Container::Menu::ListItem::onKeyDown(sf::Keyboard::Key key){
		if (key == sf::Keyboard::Return){
			menu->addObject(menu->createObject(str));
			menu->close();
		} else if (key == sf::Keyboard::Down){
			focusToNextWindow();
		} else if (key == sf::Keyboard::Up){
			focusToPreviousWindow();
		}
	}
	void Container::Menu::ListItem::render(sf::RenderWindow& rw){
		sf::RectangleShape rect;
		rect.setFillColor(sf::Color(isFocused() ? 0x808000FF : 0xFF));
		rect.setSize(size);
		rect.setOutlineColor(sf::Color(0x404040FF));
		rect.setOutlineThickness(1.0f);
		rw.draw(rect);
		renderChildWindows(rw);
	}

	// Master Container
	void MasterContainer::render(sf::RenderWindow& rw){
		pos = {0, 0};
		size = ui::getScreenSize();
		Container::render(rw);
	}

} // namespace fui