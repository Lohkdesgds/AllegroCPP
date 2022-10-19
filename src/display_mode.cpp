#include "../include/display_mode.h"

namespace AllegroCPP {

	Display_mode::Display_mode()
	{
		if (!al_is_system_installed()) al_init();

		for (int ix = 0; ix < al_get_num_display_modes(); ++ix) {
			ALLEGRO_DISPLAY_MODE mode;
			al_get_display_mode(ix, &mode);
			m_modes.push_back(std::move(mode));
		}
	}

}