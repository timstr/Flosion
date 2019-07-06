#pragma once

#include "Stateful.h"

namespace flo {

	struct SoundSource;

	struct SoundInput : Stateful {
		// parent is the stateful object to which the input belongs
		SoundInput(Stateful* _parent) noexcept;
		virtual ~SoundInput() noexcept;

		// return the currently connected sound source
		SoundSource* SoundInput::getSource() const noexcept;

		// connect a new sound source
		void setSource(SoundSource* _source) noexcept;

	protected:

		// the soundsource to which the input is connected and gets sound data from
		// null when disconnected
		SoundSource* m_source;

		// the object to which the input belongs
		Stateful* m_parent;
	};

} // namespace flo