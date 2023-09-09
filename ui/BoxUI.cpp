#include "BoxUI.h"
#include "Font.h"
#include "ObjectFactory.h"
#include <GUI/Helpers.hpp>
#include <NumberWireUI.h>
#include <SoundWireUI.h>

namespace fui {

	struct Box::InputPanel : ui::FreeElement {
		InputPanel(Box& box) :
			m_box(box) {

			setWidth(panel_width, true);
			setBackgroundColor(sf::Color(0xFFFFFF80));
			setBorderColor(sf::Color(0x808080FF));
			setBorderThickness(1);
			setBorderRadius(5);

			auto onSubmit = [this](const std::wstring& str){
				// TODO: allow arithmetic expressions
				// TODO: allow ranges, values, and names of variable to be specified
				// TODO: consider using Boost Spirit to create parsers for the above
				if (!createObject({ str.begin(), str.end() })){
					createObject(first_result);
				}
				close();
			};

			auto validate = std::function<bool(const std::wstring&)>{};

			auto onType = [this](const std::wstring& str){
				refreshResults({ str.begin(), str.end() });
			};

			auto div = add<ui::BlockElement>();
			textfield = div->add<ui::TextEntryHelper>("", getFont(), onSubmit, validate, onType);
			auto textentry = textfield->textEntry();
			textentry->setBackgroundColor(sf::Color(0x0));
			results = ui::create<ui::BlockElement>();
			results->setBackgroundColor(sf::Color(0x202020FF));
			results->setBorderColor(sf::Color(0x808080FF));
			results->setBorderThickness(1);
			results->setBorderRadius(5);
		}

		void onFocus() override {
			textfield->textEntry()->startTyping();
		}

		void onLoseFocus() override {
			close();
		}

		bool onKeyDown(ui::Key key) override {
			if (key == ui::Key::Down){
				results->grabFocus();
				results->navigateIn();
				return true;
			} else if (key == ui::Key::Escape){
				close();
				return true;
			}
			textfield->textEntry()->startTyping();
			return true;
		}

		bool createObject(std::string str){
			auto obj = Factory::createObject(str);
			if (obj){
				m_box.addObject(obj);
				obj->setPos({left() + (width() - obj->width()) * 0.5f, top()});
			}
			return (bool)obj;
		}

		void refreshResults(std::string str){
			adopt(results);
			results->clear();
			auto& map = Factory::getObjectCreators();
			first_result = "";
			int count = 0;
			if (str.size() > 0){
				for (auto it = map.begin(), end = map.end(); it != end && count < panel_max_results; ++it){
					const auto& keystr = it->first;
					if (keystr.find(str) != std::string::npos){
						if (count == 0){
							first_result = keystr;
						}
						results->add<Result>(*this, keystr);
						auto br = results->add<ui::BlockElement>();
						br->setBorderThickness(1);
						br->setMargin(1);
						br->setBorderColor(sf::Color(0x808080FF));
						br->disable();
						++count;
					}
				}
			}
			if (count == panel_max_results){
				results->write("...", getFont(), sf::Color(0x808080FF));
			}
			if (count == 0){
				results->write("no results", getFont(), sf::Color(0x808080FF), 15, ui::TextStyle::Italic);
			}
		}

		struct Result : ui::BlockElement {
			Result(InputPanel& panel, std::string str) :
				m_panel(panel),
				m_string(str) {

				setBorderRadius(5);
				setPadding(1);

				write(str, getFont(), sf::Color(0xFFFFFFFF));
			}

			void onMouseOver() override {
				setBackgroundColor(sf::Color(0x606020FF));
			}

			void onMouseOut() override {
				setBackgroundColor(sf::Color(0x202020FF));
			}

			void onFocus() override {
				setBackgroundColor(sf::Color(0x606020FF));
			}

			void onLoseFocus() override {
				setBackgroundColor(sf::Color(0x202020FF));
			}

			bool onLeftClick(int clicks) override {
				m_panel.createObject(m_string);
				m_panel.close();
				return true;
			}

			bool onKeyDown(ui::Key key) override {
				if (key == ui::Key::Enter){
					m_panel.createObject(m_string);
					m_panel.close();
					return true;
				} else if (key == ui::Key::Up){
					navigateToPreviousElement();
					return true;
				} else if (key == ui::Key::Down){
					navigateToNextElement();
					return true;
				}
				return false;
			}

			InputPanel& m_panel;
			const std::string m_string;
		};

		ui::Ref<ui::TextEntryHelper> textfield;
		ui::Ref<ui::BlockElement> results;
		std::string first_result;
		Box& m_box;

		const static float panel_width;
		const static int panel_max_results;
	};
	const float Box::InputPanel::panel_width = 150.0f;
	const int Box::InputPanel::panel_max_results = 10;


	void Box::shiftContents(vec2 delta)	{
		for (const auto& obj: m_objects){
			obj->setLeft(obj->left() + delta.x);
			obj->setTop(obj->top() + delta.y);
		}
		for (const auto& wire: m_numberwires){
			wire->setPos(wire->pos() + delta);
		}
		for (const auto& wire: m_soundwires){
			wire->setPos(wire->pos() + delta);
		}
	}

	std::pair<vec2, vec2> Box::getContentBounds() {
		if (m_objects.empty()){
			return {{0, 0}, {0, 0}};
		}

		vec2 minp = m_objects.front()->pos();
		vec2 maxp = minp;

		for (const auto& obj : m_objects){
			vec2 tl = obj->pos();
			vec2 br = tl + obj->size();
			minp.x = std::min(minp.x, tl.x);
			minp.y = std::min(minp.y, tl.y);
			maxp.x = std::max(maxp.x, br.x);
			maxp.y = std::max(maxp.y, br.y);
		}

		return {minp, maxp};
	}

	bool Box::onLeftClick(int clicks) {
		if (clicks == 2){
			auto input = add<InputPanel>(*this);
			input->setPos(localMousePos() - vec2(input->width() * 0.5f, 0));
			input->grabFocus();
		} else {
			// TODO: start dragging selection box
		}
		return true;
	}

	Box::Box(){
		m_object_container = add<ui::FreeElement>();
		m_wire_container = add<ui::FreeElement>();
	}

	void Box::addObject(ui::Ref<Object> object) {
		m_objects.push_back(object);
		object->m_parentbox = this->thisAs<Box>();
		m_object_container->adopt(object);
	}

	void Box::releaseObject(ui::Ref<Object> object) {
		m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), object), m_objects.end());
		object->m_parentbox.reset();
		m_object_container->release(object);
	}

	void Box::addSoundWire(ui::Ref<SoundWire> wire) {
		m_soundwires.push_back(wire);
		m_wire_container->adopt(wire);
	}

	void Box::removeSoundWire(SoundWire& wire) {
		for (auto it = m_soundwires.begin(); it != m_soundwires.end();) {
			if (it->get() == &wire){
				m_wire_container->release(*it);
				it = m_soundwires.erase(it);
			} else {
				++it;
			}
		}
	}

	void Box::addNumberWire(ui::Ref<NumberWire> wire) {
		m_numberwires.push_back(wire);
		m_wire_container->adopt(wire);
	}

	void Box::removeNumberWire(NumberWire& wire) {
		for (auto it = m_numberwires.begin(); it != m_numberwires.end();) {
			if (it->get() == &wire){
				m_wire_container->release(*it);
				it = m_numberwires.erase(it);
			} else {
				++it;
			}
		}
	}

	CollapsibleBox::CollapsibleBox() :
		m_iscollapsed(false) {
		setMinWidth(100);
		m_titlebar = add<ui::BlockElement>();
		m_titlebar->setBackgroundColor(sf::Color(0xCC333380));
		m_titlebar->setBorderRadius(3);
		m_titlebar->write("Box!", getFont(), sf::Color(0xFF), 15, ui::TextStyle::Bold | ui::TextStyle::Italic);
		m_titlebar->setMargin(2);
		m_titlebar->setContentAlign(ui::ContentAlign::Center);
		m_box = add<Box>();
		m_box->setLayoutStyle(ui::LayoutStyle::Block);
		m_box->setMinHeight(100);
		m_box->setBackgroundColor(sf::Color(0x40404080));
		m_box->setMargin(2);
	}

	void CollapsibleBox::collapse() {
		// TODO
		if (m_iscollapsed){
			return;
		}
		m_iscollapsed = true;
		release(m_box);
	}

	void CollapsibleBox::expand() {
		// TODO
		if (!m_iscollapsed){
			return;
		}
		m_iscollapsed = false;
		adopt(m_box);
	}

	void CollapsibleBox::render(sf::RenderWindow& rw) {
		if (!m_iscollapsed){
			auto [minp, maxp] = m_box->getContentBounds();
			const vec2 margin = {30.0f, 30.0f};
			if (abs(minp.x - maxp.x) + abs(minp.y - maxp.y) > 0){
				vec2 delta = margin - minp;
				m_box->shiftContents(delta);
				setPos(pos() - delta);
			}
			m_box->setSize(maxp - minp + 2.0f * margin, true);
			m_titlebar->setWidth(m_box->width(), true);
		}
		Element::render(rw);
	}

	bool CollapsibleBox::onLeftClick(int clicks) {
		if (clicks == 2){
			if (m_iscollapsed){
				expand();
			} else {
				collapse();
			}
			return true;
		}
		startDrag();
		return true;
	}

	void CollapsibleBox::onLeftRelease(){
		stopDrag();
	}

	MainBox::MainBox() {
		//setBorderRadius(0);
		setMinSize({500, 500});
		setBackgroundColor(sf::Color(0x404040FF));
	}

	void MainBox::render(sf::RenderWindow& rw) {
		resizeToFit(ui::getScreenSize());
		Box::render(rw);
	}

	void MainBox::resizeToFit(vec2 screensize) {
		// if the mainbox's top left corner is below or right of the window's top left
		// corner, move the mainbox to the corner and shift every object over by the
		// difference
		float shift_x = std::max(0.0f, left());
		float shift_y = std::max(0.0f, top());
		setLeft(left() - shift_x);
		setTop(top() - shift_y);

		shiftContents({shift_x, shift_y});

		// if the mainbox's bottom right corner is left or above the window's bottom
		// right corner, simply resize the mainbox by the difference
		auto resize_x = std::max(0.0f, screensize.x - left() - width());
		auto resize_y = std::max(0.0f, screensize.y - top() - height());
		setMinWidth(width() + resize_x);
		setMinHeight(height() + resize_y);
	}

} // namespace fui