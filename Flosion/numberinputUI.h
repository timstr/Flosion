#pragma once

#include <gui/gui.h>
#include "NumberInput.h"
#include "NumberSource.h"
#include "numberoutputUI.h"
#include "numberwireUI.h"

namespace fui {

	// the destination for a numberwire
	// corresponds to the input of a processing object
	struct NumberInput : ui::Window {
		NumberInput(musical::NumberInput* _target, Object* _parent, const std::string& _caption = "", const std::function<void(NumberOutput*)>& _onConnect = {});
		~NumberInput();

		void setWireIn(NumberWire* wire);

		bool onDropWindow(Window* window) override;

		void onHover() override;
		void onHoverWithWindow(Window* win) override;

		void render(sf::RenderWindow& rw) override;

		void onLeftClick(int clicks) override;

		const std::string& getCaption() const;

		const musical::NumberInput* getInput() const;

		vec2 getWireDirection() const;

		private:
		Object* const owner_object;
		musical::NumberInput* const target;
		NumberWire* wire_in;
		ui::Text* caption;
		std::string caption_str;
		double hover_timestamp;
		std::function<void(NumberOutput*)> onConnect;

		friend struct NumberWire;
	};

} // namespace fui