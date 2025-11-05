#include "monitor_info.h"

namespace AllegroCPP {

	Monitor_info::Monitor_info(int adapter)
	{
		if (!al_is_system_installed()) al_init();

		if (adapter < 0) adapter = 0;
		if (adapter >= al_get_num_display_modes()) adapter = al_get_num_display_modes() - 1;

		int _old_targ = al_get_new_display_adapter();
		al_set_new_display_adapter(adapter);

		al_get_monitor_info(adapter, &m_info);
		m_display_adapter = al_get_new_display_adapter();
		m_dpi = al_get_monitor_dpi(adapter);
		m_refresh_rate = al_get_new_display_refresh_rate();

		al_set_new_display_adapter(_old_targ);
	}

	int Monitor_info::get_width() const
	{
		int res = m_info.x2 - m_info.x1;
		return res < 0 ? -res : res;
	}

	int Monitor_info::get_height() const
	{
		int res = m_info.y2 - m_info.y1;
		return res < 0 ? -res : res;
	}

}