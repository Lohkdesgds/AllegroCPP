#pragma once

#include <allegro5/allegro.h>

#include <stdexcept>
#include <string>

namespace AllegroCPP {

	class Mouse {
		ALLEGRO_MOUSE_STATE m_state{};
	public:
		enum class axis { X_AXIS, Y_AXIS, Z_AXIS };

		Mouse();

		void update();

		static unsigned int get_num_axes();
		static unsigned int get_num_buttons();

		int get_axis(const axis&) const;
		int get_axis(const int) const;

		// index starts at 1 in this case
		bool get_button_down(const int) const;

		ALLEGRO_MOUSE_STATE get() const;

		static bool set_mouse_pos(const int, const int, ALLEGRO_DISPLAY* = nullptr);
		static bool set_mouse_pos(const std::pair<int, int>, ALLEGRO_DISPLAY* = nullptr);
		static bool set_mouse_pos_z(const int);
		static bool set_mouse_pos_w(const int);
		static bool set_mouse_axis(const axis&, const int);
		static bool set_mouse_axis(const int, const int);
	};

	class Mouse_cursor {
		ALLEGRO_MOUSE_CURSOR* m_cursor = nullptr;
	public:
		Mouse_cursor(const Mouse_cursor&) = delete;
		Mouse_cursor(Mouse_cursor&&) noexcept;
		void operator=(const Mouse_cursor&) = delete;
		void operator=(Mouse_cursor&&) noexcept;

		Mouse_cursor(ALLEGRO_BITMAP*, const int, const int);
		~Mouse_cursor();

		void destroy();

		static bool set(ALLEGRO_DISPLAY* display, ALLEGRO_SYSTEM_MOUSE_CURSOR cursor_id);

		// real screen pos, not related to display
		static bool get_pos(int& x, int& y);
		static std::pair<int, int> get_pos();

		static bool hide(ALLEGRO_DISPLAY* display = nullptr);
		static bool show(ALLEGRO_DISPLAY* display = nullptr);

		static bool grab(ALLEGRO_DISPLAY* display = nullptr);
		static bool ungrab();
	};

}