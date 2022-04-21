#pragma once

#include <allegro5/allegro.h>

namespace AllegroCPP {

	struct Monitor_info {
		ALLEGRO_MONITOR_INFO m_info{};
		int m_display_adapter = -1;
		int m_dpi = 0;
		int m_refresh_rate = 0;

		Monitor_info(int adapter = ALLEGRO_DEFAULT_DISPLAY_ADAPTER);

		int get_width() const;
		int get_height() const;
	};

}