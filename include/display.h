#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_windows.h>
#include <Windows.h>

#include <vector>
#include <stdexcept>
#include <string>
#include <memory>

#ifdef _WIN32
#include <allegro5/allegro_windows.h>
#endif

#undef max
#undef min

namespace AllegroCPP {

	constexpr int display_undefined_position[2] = { std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };

	struct display_option {
		int option;
		int value;
		int importance; // ALLEGRO_REQUIRE|SUGGEST|DONTCARE
	};

	class Display {
		std::shared_ptr<ALLEGRO_DISPLAY> m_disp;
	public:
		Display(const int size_x, const int size_y, const std::string& windowname, const int flags = 0,
			const int pos_x = display_undefined_position[0], const int pos_y = display_undefined_position[1],
			const int refresh_rate = 0, const std::vector<display_option> options = {},
			const bool centered_on_pos_x = false, const bool centered_on_pos_y = false);
		~Display();

		Display(const Display&) = delete;
		Display(Display&&) noexcept;
		void operator=(const Display&) = delete;
		void operator=(Display&&) noexcept;

		bool create(const int size_x, const int size_y, const std::string& windowname, const int flags = 0,
			const int pos_x = display_undefined_position[0], const int pos_y = display_undefined_position[1],
			const int refresh_rate = 0, const std::vector<display_option> options = {},
			const bool centered_on_pos_x = false, const bool centered_on_pos_y = false);
		void destroy();

		bool empty() const;
		bool valid() const;
		operator bool() const;
		operator ALLEGRO_DISPLAY*();
		operator ALLEGRO_EVENT_SOURCE*();
		operator std::weak_ptr<ALLEGRO_DISPLAY>();

		ALLEGRO_EVENT_SOURCE* get_event_source() const;
		ALLEGRO_BITMAP* get_backbuffer();
		ALLEGRO_DISPLAY* get_display();
		std::weak_ptr<ALLEGRO_DISPLAY> get_display_ref();

		bool flip();
		bool update_region(const int pos_x, const int pos_y, const int width, const int height);
		void wait_for_vsync() const;

		int get_width() const;
		int get_height() const;
		bool resize(const int width, const int height);
		void acknowledge_resize();

		bool get_position(int& x, int& y) const;
		bool set_position(const int pos_x, const int pos_y);
		bool get_constraints(int& min_x, int& min_y, int& max_x, int& max_y) const;
		bool set_constraints(const int min_x, const int min_y, const int max_x, const int max_y);
		bool apply_constraints(bool);

		int get_flags() const;
		bool set_flag(const int, const bool);
		int get_option(const int) const;
		bool set_option(const int, const int);

		int get_format() const;
		int get_orientation() const;
		int get_refresh_rate() const;

		bool set_title(const std::string&);

		bool set_icon(ALLEGRO_BITMAP*);
		bool set_icons(std::vector<ALLEGRO_BITMAP*>);

		bool acknowledge_drawing_halt();
		bool acknowledge_drawing_resume();

		static void inhibit_screensaver(const bool);

		bool get_has_clipboard_text() const;
		std::string get_clipboard_text() const;
		bool clear_clipboard_text();
		bool set_clipboard_text(const std::string&);

		bool set_as_target();

		bool set_clip_rectangle(const int pos_x, const int pos_y, const int width, const int height);
		bool get_clip_rectangle(int& pos_x, int& pos_y, int& width, int& height) const;
		bool reset_clip_rectangle();

		const ALLEGRO_TRANSFORM* get_current_transform() const;
		const ALLEGRO_TRANSFORM* get_current_inverse_transform() const;
		const ALLEGRO_TRANSFORM* get_current_projection_transform() const;

		bool clear_to_color(const ALLEGRO_COLOR = al_map_rgb(0,0,0));

		bool set_blend_color(const ALLEGRO_COLOR);
		bool set_blender(const int op, const int src, const int dst);
		bool set_blender(const int op, const int src, const int dst, const int alpha_op, const int alpha_src, const int alpha_dst);
		bool get_blend_color(ALLEGRO_COLOR&);
		bool get_blender(int& op, int& src, int& dst);
		bool get_blender(int& op, int& src, int& dst, int& alpha_op, int& alpha_src, int& alpha_dst);

		void operator<<(ALLEGRO_MENU*);
#ifdef _WIN32
		bool set_icon_from_resource(const int id);
		HWND get_window_handler();

		bool make_window_masked(ALLEGRO_COLOR);
		bool unmake_window_masked();
#endif
	};
}