#include "boxUI.h"
#include "Font.h"

namespace fui {

	bool Box::onLeftClick(int clicks) {
		if (clicks == 2){
			// TODO: open typing menu
		} else {
			// TODO: start dragging selection box
		}
		return true;
	}

	void Box::addObject(ui::Ref<Object> object) {
		m_objects.push_back(object);
		adopt(object);
	}

	void Box::releaseObject(ui::Ref<Object> object) {
		m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), object), m_objects.end());
		release(object);
	}

	void CollapsibleBox::collapse() {
		// TODO
	}

	void CollapsibleBox::expand() {
		// TODO
	}

	MainBox::MainBox() {
		write("Main Box haha!", getFont());
		setMinSize({100, 100});
		setBackgroundColor(sf::Color(0x404040FF));
	}

} // namespace fui