#pragma once

#include <Flosion/UI/Core/Object.hpp>
#include <Flosion/UI/Core/Box.hpp>
#include <Flosion/UI/Core/NumberWire.hpp>
#include <Flosion/UI/Core/SoundWire.hpp>

namespace flui {

	// The entry point of it all
	class FlosionUI : ui::FreeElement {
		FlosionUI();

	private:

		void moveWithMouse();

		void render(sf::RenderWindow& rw) override;

		ui::Ref<MainBox> box;

		// time that mouse first moved to border
		// when this passes a threshold, the main box begins moving with the mouse
		float mouse_timestamp;
	};

}