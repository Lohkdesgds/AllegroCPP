#include "../include/display.h"

namespace AllegroCPP {

	Display::Display(std::pair<int, int> size, const std::string& windowname, int flags, std::pair<int, int> pos, int refresh_rate, std::vector<display_option> options)
	{
		if (!al_is_system_installed()) al_init();

		if (flags >= 0) al_set_new_display_flags(flags);
		if (pos != display_invalid_position) al_set_new_window_position(pos.first, pos.second);
		if (refresh_rate > 0) al_set_new_display_refresh_rate(refresh_rate);
		for (const auto& i : options) al_set_new_display_option(i.option, i.value, i.importance);
		al_set_new_window_title(windowname.substr(0, ALLEGRO_NEW_WINDOW_TITLE_MAX_SIZE).c_str());

		m_disp = al_create_display(size.first, size.second);
		if (!m_disp) throw std::runtime_error("Failed creating display!");
	}

	Display::~Display()
	{
		if (m_disp) al_destroy_display(m_disp);
		m_disp = nullptr;
	}

	Display::Display(Display&& oth) noexcept
		: m_disp(oth.m_disp)
	{
		oth.m_disp = nullptr;
	}

	void Display::operator=(Display&& oth) noexcept
	{
		if (m_disp) al_destroy_display(m_disp);
		m_disp = oth.m_disp;
		oth.m_disp = nullptr;
	}

	bool Display::empty() const
	{
		return m_disp == nullptr;
	}

	bool Display::valid() const
	{
		return m_disp != nullptr;
	}

	Display::operator bool() const
	{
		return valid();
	}

	Display::operator ALLEGRO_DISPLAY* ()
	{
		return m_disp;
	}

	ALLEGRO_EVENT_SOURCE* Display::get_event_source() const
	{
		return m_disp ? al_get_display_event_source(m_disp) : nullptr;
	}

	ALLEGRO_BITMAP* Display::get_backbuffer()
	{
		return m_disp ? al_get_backbuffer(m_disp) : nullptr;
	}

	ALLEGRO_DISPLAY* Display::get_display()
	{
		return m_disp;
	}

	bool Display::flip()
	{
		if (!m_disp) return false;
		if (al_get_current_display() != m_disp) al_set_target_backbuffer(m_disp);
		al_flip_display();
		return true;
	}

	bool Display::update_region(std::pair<int, int> pos, std::pair<int, int> size)
	{
		if (!m_disp) return false;
		if (al_get_current_display() != m_disp) al_set_target_backbuffer(m_disp);
		al_update_display_region(pos.first, pos.second, size.first, size.second);
		return true;
	}

	void Display::wait_for_vsync() const
	{
		if (!m_disp) return;
		if (al_get_current_display() != m_disp) al_set_target_backbuffer(m_disp);
		al_wait_for_vsync();
	}

	int Display::get_width() const
	{
		return m_disp ? al_get_display_width(m_disp) : -1;
	}

	int Display::get_height() const
	{
		return m_disp ? al_get_display_height(m_disp) : -1;
	}

	bool Display::resize(std::pair<int, int> size)
	{
		if (!m_disp) return false;
		if (!al_resize_display(m_disp, size.first, size.second)) return false;
		return true;
	}

	void Display::acknowledge_resize()
	{
		if (m_disp) al_acknowledge_resize(m_disp);
	}

	std::pair<int, int> Display::get_position() const
	{
		if (!m_disp) return display_invalid_position;
		std::pair<int, int> pos;
		al_get_window_position(m_disp, &pos.first, &pos.second);
		return pos;
	}

	bool Display::set_position(std::pair<int, int> pos)
	{
		if (!m_disp) return false;
		al_set_window_position(m_disp, pos.first, pos.second);
		return true;
	}

	bool Display::get_constraints(int& minx, int& miny, int& maxx, int& maxy) const
	{
		if (!m_disp) return false;
		al_get_window_constraints(m_disp, &minx, &miny, &maxx, &maxy);
		return true;
	}

	bool Display::set_constraints(std::pair<int, int> min, std::pair<int, int> max)
	{
		if (!m_disp) return false;
		al_set_window_constraints(m_disp, min.first, min.second, max.first, max.second);
		return true;
	}

	bool Display::apply_constraints(bool enab)
	{
		if (!m_disp) return false;
		al_apply_window_constraints(m_disp, enab);
		return true;
	}

	int Display::get_flags() const
	{
		return m_disp ? al_get_display_flags(m_disp) : -1;
	}

	bool Display::set_flag(int flag, bool enabled)
	{
		if (!m_disp) return false;
		return al_set_display_flag(m_disp, flag, enabled);
	}

	int Display::get_option(int opt) const
	{
		return m_disp ? al_get_display_option(m_disp, opt) : -1;
	}

	bool Display::set_option(int opt, int val)
	{
		if (!m_disp) return false;
		al_set_display_option(m_disp, opt, val);
		return true;
	}

	int Display::get_format() const
	{
		return m_disp ? al_get_display_format(m_disp) : -1;
	}

	int Display::get_orientation() const
	{
		return m_disp ? al_get_display_orientation(m_disp) : -1;
	}

	int Display::get_refresh_rate() const
	{
		return m_disp ? al_get_display_refresh_rate(m_disp) : -1;
	}

	bool Display::set_title(const std::string& titl)
	{
		if (!m_disp) return false;
		al_set_window_title(m_disp, titl.substr(0, ALLEGRO_NEW_WINDOW_TITLE_MAX_SIZE).c_str());
		return true;
	}

	bool Display::set_icon(ALLEGRO_BITMAP* bitmap)
	{
		if (!m_disp || !bitmap) return false;
		al_set_display_icon(m_disp, bitmap);
		return true;
	}

	bool Display::set_icons(std::vector<ALLEGRO_BITMAP*> vec)
	{
		if (!m_disp || vec.size() == 0 || std::find(vec.begin(), vec.end(), nullptr) != vec.end()) return false;
		al_set_display_icons(m_disp, static_cast<int>(vec.size()), vec.data());
		return true;
	}

	bool Display::acknowledge_drawing_halt()
	{
		if (!m_disp) return false;
		al_acknowledge_drawing_halt(m_disp);
		return true;
	}

	bool Display::acknowledge_drawing_resume()
	{
		if (!m_disp) return false;
		al_acknowledge_drawing_resume(m_disp);
		return true;
	}

	void Display::inhibit_screensaver(bool inhibit)
	{
		al_inhibit_screensaver(inhibit);
	}

	bool Display::get_has_clipboard_text() const
	{
		if (!m_disp) return false;
		return al_clipboard_has_text(m_disp);
	}

	std::string Display::get_clipboard_text() const
	{
		if (!m_disp) return {};
		const char* str = al_clipboard_has_text(m_disp) ? al_get_clipboard_text(m_disp) : nullptr;
		return str ? str : std::string{};
	}

	bool Display::clear_clipboard_text()
	{
		if (!m_disp) return false;
		return al_set_clipboard_text(m_disp, nullptr);
	}

	bool Display::set_clipboard_text(const std::string& str)
	{
		if (!m_disp) return false;
		return al_set_clipboard_text(m_disp, str.c_str());
	}

	bool Display::set_as_target()
	{
		if (!m_disp) return false;
		al_set_target_backbuffer(m_disp);
		return true;
	}

	bool Display::set_clip_rectangle(std::pair<int, int> clipcut, std::pair<int, int> clipsize)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp);
		al_set_clipping_rectangle(clipcut.first, clipcut.second, clipsize.first, clipsize.second);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::get_clip_rectangle(int& posx, int& posy, int& width, int& height) const
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp);
		al_get_clipping_rectangle(&posx, &posy, &width, &height);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::reset_clip_rectangle()
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp);
		al_reset_clipping_rectangle();
		al_set_target_bitmap(oldtarg);
		return true;
	}

	const ALLEGRO_TRANSFORM* Display::get_current_transform() const
	{
		if (!m_disp) return nullptr;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		const ALLEGRO_TRANSFORM* currt = al_get_current_transform();
		al_set_target_bitmap(oldtarg);
		return currt;
	}

	const ALLEGRO_TRANSFORM* Display::get_current_inverse_transform() const
	{
		if (!m_disp) return nullptr;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		const ALLEGRO_TRANSFORM* currt = al_get_current_inverse_transform();
		al_set_target_bitmap(oldtarg);
		return currt;
	}

	const ALLEGRO_TRANSFORM* Display::get_current_projection_transform() const
	{
		if (!m_disp) return nullptr;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		const ALLEGRO_TRANSFORM* currt = al_get_current_projection_transform();
		al_set_target_bitmap(oldtarg);
		return currt;
	}

}