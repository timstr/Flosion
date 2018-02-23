#pragma once

#include <gui/gui.h>
#include "NumberSource.h"
#include "NumberInput.h"
#include "numberinputUI.h"
#include "numberwireUI.h"

namespace fui {

	// the source of a numberwire
	// corresponds to a number source
	struct NumberOutput : ui::Window {
		NumberOutput(musical::NumberSource* _target, Object* _parent, const std::string& _caption = "", const std::function<void(NumberInput*)>& _onConnect = {});
		~NumberOutput();

		void addWireOut(NumberWire* wire);
		void removeWireOut(NumberWire* wire);

		void render(sf::RenderWindow& rw) override;

		bool onDropWindow(Window* window) override;

		void onHover() override;
		void onHoverWithWindow(Window* win) override;

		void onLeftClick(int clicks) override;

		const std::string& getCaption() const;

		const musical::NumberSource* getSource() const;

		private:
		Object* const owner_object;
		musical::NumberSource* const target;
		std::vector<NumberWire*> wires_out;
		ui::Text* caption;
		std::string caption_str;
		double hover_timestamp;
		std::function<void(NumberInput*)> onConnect;

		friend struct NumberWire;
	};

} // namespace fui