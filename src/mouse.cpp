#include "../include/mouse.h"

namespace AllegroCPP {

	Mouse::Mouse()
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_mouse_installed()) al_install_mouse();
		update();
	}

	void Mouse::update()
	{
		al_get_mouse_state(&m_state);

		// more robust tracking, get info out of screen too (relative to screen)
		if (auto* dsp = al_get_current_display(); dsp) {
			al_get_mouse_cursor_position(&m_state.x, &m_state.y);
			int dx = 0, dy = 0;
			al_get_window_position(dsp, &dx, &dy);
			m_state.x -= dx;
			m_state.y -= dy;

			m_out_of_screen = m_state.x < 0 || m_state.y < 0 || m_state.x >= al_get_display_width(dsp) || m_state.y >= al_get_display_height(dsp);
		}
		else m_out_of_screen = false; // not supported
	}

	unsigned int Mouse::get_num_axes()
	{
		return al_get_mouse_num_axes();
	}

	unsigned int Mouse::get_num_buttons()
	{
		return al_get_mouse_num_buttons();
	}

	int Mouse::get_axis(const Mouse::axis& axis) const
	{
		switch (axis) {
		case Mouse::axis::X_AXIS:
			return al_get_mouse_state_axis(&m_state, 0);
		case Mouse::axis::Y_AXIS:
			return al_get_mouse_state_axis(&m_state, 1);
		case Mouse::axis::Z_AXIS:
			return al_get_mouse_state_axis(&m_state, 2);
		}
		return 0;
	}

	int Mouse::get_axis(const int axis) const
	{
		return al_get_mouse_state_axis(&m_state, axis);
	}

	bool Mouse::is_out_of_screen() const
	{
		return m_out_of_screen;
	}

	bool Mouse::get_button_down(const int btn) const
	{
		return al_mouse_button_down(&m_state, btn);
	}

	ALLEGRO_MOUSE_STATE Mouse::get() const
	{
		return m_state;
	}

	bool Mouse::set_mouse_pos(const int pos_x, const int pos_y, ALLEGRO_DISPLAY* d)
	{
		if (!d && !(d = al_get_current_display())) return false;
		return al_set_mouse_xy(d, pos_x, pos_y);
	}

	bool Mouse::set_mouse_pos_z(const int z)
	{
		return al_set_mouse_z(z);
	}

	bool Mouse::set_mouse_pos_w(const int z)
	{
		return al_set_mouse_z(z);
	}

	bool Mouse::set_mouse_axis(const axis& axis, const int value)
	{
		switch (axis) {
		case Mouse::axis::X_AXIS:
			return al_set_mouse_axis(0, value);
		case Mouse::axis::Y_AXIS:
			return al_set_mouse_axis(1, value);
		case Mouse::axis::Z_AXIS:
			return al_set_mouse_axis(2, value);
		}
		return false;
	}

	bool Mouse::set_mouse_axis(const int axis, const int value)
	{
		return al_set_mouse_axis(axis, value);
	}

	Mouse_cursor::Mouse_cursor(Mouse_cursor&& oth) noexcept
		: m_cursor(std::exchange(oth.m_cursor, nullptr))
	{
	}

	void Mouse_cursor::operator=(Mouse_cursor&& oth) noexcept
	{
		destroy();
		m_cursor = std::exchange(oth.m_cursor, nullptr);
	}

	Mouse_cursor::Mouse_cursor(ALLEGRO_BITMAP* bmp, const int x_focus, const int y_focus)
	{
		if (!bmp) throw std::runtime_error("Bitmap MUST NOT be null");
		m_cursor = al_create_mouse_cursor(bmp, x_focus, y_focus);
	}

	Mouse_cursor::~Mouse_cursor()
	{
		destroy();
	}

	void Mouse_cursor::destroy()
	{
		if (m_cursor) {
			al_destroy_mouse_cursor(m_cursor);
			m_cursor = nullptr;
		}
	}

	bool Mouse_cursor::set(ALLEGRO_DISPLAY* display, ALLEGRO_SYSTEM_MOUSE_CURSOR cursor_id)
	{
		if (!display && !(display = al_get_current_display())) return false;
		return al_set_system_mouse_cursor(display, cursor_id);
	}

	bool Mouse_cursor::get_pos(int& x, int& y)
	{
		return al_get_mouse_cursor_position(&x, &y);
	}

	int Mouse_cursor::get_pos_x()
	{
		int x, y;
		if (!al_get_mouse_cursor_position(&x, &y)) return 0;
		return x;
	}

	int Mouse_cursor::get_pos_y()
	{
		int x, y;
		if (!al_get_mouse_cursor_position(&x, &y)) return 0;
		return y;
	}

	bool Mouse_cursor::hide(ALLEGRO_DISPLAY* display)
	{
		if (!display && !(display = al_get_current_display())) return false;
		return al_hide_mouse_cursor(display);
	}

	bool Mouse_cursor::show(ALLEGRO_DISPLAY* display)
	{
		if (!display && !(display = al_get_current_display())) return false;
		return al_show_mouse_cursor(display);
	}

	bool Mouse_cursor::grab(ALLEGRO_DISPLAY* display)
	{
		if (!display && !(display = al_get_current_display())) return false;
		return al_grab_mouse(display);
	}

	bool Mouse_cursor::ungrab()
	{
		return al_ungrab_mouse();
	}

}