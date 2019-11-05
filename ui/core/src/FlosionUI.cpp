#include <Flosion/UI/Core/FlosionUI.hpp>

#include <Flosion/UI/Core/ObjectFactory.hpp>

namespace flui {

	FlosionUI::FlosionUI()
		: GridContainer(1, 1)
        , m_panel(putCell<MainPanel>(0, 0)) {
		
	}

} // namespace flui
