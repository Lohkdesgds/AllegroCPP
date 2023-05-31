#pragma once

#include <vector>

#include <allegro5/allegro.h>

namespace AllegroCPP {

	struct Display_mode {
		std::vector<ALLEGRO_DISPLAY_MODE> m_modes;
		Display_mode();
	};

}