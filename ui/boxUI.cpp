#include "boxUI.h"
#include "Font.h"
#include "FlosionUI.h"
#include <GUI/Helpers.hpp>

namespace fui {

	struct Box::InputPanel : ui::FreeElement {
		InputPanel(Box& box) :
			m_box(box) {

			setMinSize({100, 25});
			setBackgroundColor(sf::Color(0xFFFFFF80));

			auto onSubmit = [this](const std::wstring& str){
				createAndClose({ str.begin(), str.end() });
			};

			auto validate = [](const std::wstring&) -> bool {
				return true;
			};

			auto onType = [this](const std::wstring& str){
				refreshResults({ str.begin(), str.end() });
			};

			auto div = add<ui::BlockElement>();
			textfield = div->add<ui::TextEntryHelper>("", getFont(), onSubmit, validate, onType);
			textfield->setBackgroundColor(sf::Color(0));
			results = add<ui::BlockElement>();
		}

		void onFocus() override {
			textfield->textEntry()->startTyping();
		}

		void onLoseFocus() override {
			close();
		}

		bool onKeyDown(ui::Key key) override {
			if (key == ui::Key::Down){
				return true;
			} else if (key == ui::Key::Escape){
				close();
				return true;
			}
			return false;
		}

		void createAndClose(std::string str){
			auto obj = Factory::createObject(str);
			if (obj){
				m_box.addObject(obj);
				obj->setPos({left() + (width() - obj->width()) * 0.5f, top()});
			}
			close();
		}

		void refreshResults(std::string str){
			results->clear();
			auto& map = Factory::getObjectCreators();
			for (const auto& pair : map){
				const auto& keystr = pair.first;
				if (keystr.find(str) != std::string::npos){
					results->add<Result>(*this, keystr);
				}
			}
		}

		struct Result : ui::BlockElement {
			Result(InputPanel& panel, std::string str) :
				m_panel(panel),
				m_string(str) {

				write(str, getFont(), sf::Color(0xFFFFFFFF));

				setBackgroundColor(sf::Color(0x202020FF));
				setBorderColor(sf::Color(0x808080FF));
				setBorderThickness(1);
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
				m_panel.createAndClose(m_string);
				return true;
			}

			bool onKeyDown(ui::Key key) override {
				if (key == ui::Key::Return){
					m_panel.createAndClose(m_string);
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
		ui::Ref<ui::BlockElement> results; // TODO: make scroll panel
		Box& m_box;
	};


	bool Box::onLeftClick(int clicks) {
		if (clicks == 2){
			auto input = add<InputPanel>(*this);
			input->setPos(localMousePos() - input->size() * 0.5f);
			input->grabFocus();
		} else {
			// TODO: start dragging selection box
		}
		return true;
	}

	void Box::addObject(ui::Ref<Object> object) {
		m_objects.push_back(object);
		object->m_parentbox = thisAs<Box>();
		adopt(object);
	}

	void Box::releaseObject(ui::Ref<Object> object) {
		m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), object), m_objects.end());
		object->m_parentbox.reset();
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
		setMinSize({500, 500});
		setBackgroundColor(sf::Color(0x404040FF));
	}

} // namespace fui