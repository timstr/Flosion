#pragma once

#include "objectUI.h"
#include "boxUI.h"
#include "numberwireUI.h"
#include "soundwireUI.h"

namespace fui {

	// The entry point of it all
	struct FlosionUI : ui::FreeElement {
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