#include "soundwireUI.h"

namespace fui {

	ui::Ref<SoundInput::WireType> SoundInput::createWire(ui::Ref<Box> box) {
		return ui::create<SoundWire>(box);
	}

	ui::Ref<SoundOutput::WireType> SoundOutput::createWire(ui::Ref<Box> box) {
		return ui::create<SoundWire>(box);
	}

} // namespace fui