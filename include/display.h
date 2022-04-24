#pragma once

#include <allegro5/allegro.h>

#include <vector>
#include <stdexcept>
#include <string>

#undef max
#undef min

namespace AllegroCPP {

	constexpr std::pair<int, int> display_invalid_position = { std::numeric_limits<int>::min(), std::numeric_limits<int>::min() };

	struct display_option {
		int option;
		int value;
		int importance;
	};

	class Display {
		ALLEGRO_DISPLAY* m_disp = nullptr;
	public:
		Display(std::pair<int, int> size, const std::string& windowname, int flags = 0, std::pair<int, int> pos = display_invalid_position, int refresh_rate = 0, std::vector<display_option> options = {});
		~Display();

		Display(const Display&) = delete;
		Display(Display&&) noexcept;
		void operator=(const Display&) = delete;
		void operator=(Display&&) noexcept;

		bool empty() const;
		bool valid() const;
		operator bool() const;
		operator ALLEGRO_DISPLAY*();
		operator ALLEGRO_EVENT_SOURCE*();

		ALLEGRO_EVENT_SOURCE* get_event_source() const;
		ALLEGRO_BITMAP* get_backbuffer();
		ALLEGRO_DISPLAY* get_display();
		bool flip();
		bool update_region(std::pair<int, int>, std::pair<int, int>);
		void wait_for_vsync() const;

		int get_width() const;
		int get_height() const;
		bool resize(std::pair<int, int>);
		void acknowledge_resize();

		std::pair<int, int> get_position() const;
		bool set_position(std::pair<int, int>);
		bool get_constraints(int& minx, int& miny, int& maxx, int& maxy) const;
		bool set_constraints(std::pair<int, int> min, std::pair<int, int> max);
		bool apply_constraints(bool);

		int get_flags() const;
		bool set_flag(int, bool);
		int get_option(int) const;
		bool set_option(int, int);

		int get_format() const;
		int get_orientation() const;
		int get_refresh_rate() const;

		bool set_title(const std::string&);

		bool set_icon(ALLEGRO_BITMAP*);
		bool set_icons(std::vector<ALLEGRO_BITMAP*>);

		bool acknowledge_drawing_halt();
		bool acknowledge_drawing_resume();

		static void inhibit_screensaver(bool);

		bool get_has_clipboard_text() const;
		std::string get_clipboard_text() const;
		bool clear_clipboard_text();
		bool set_clipboard_text(const std::string&);

		bool set_as_target();

		bool set_clip_rectangle(std::pair<int, int> clipcut, std::pair<int, int> clipsize);
		bool get_clip_rectangle(int& posx, int& posy, int& width, int& height) const;
		bool reset_clip_rectangle();

		const ALLEGRO_TRANSFORM* get_current_transform() const;
		const ALLEGRO_TRANSFORM* get_current_inverse_transform() const;
		const ALLEGRO_TRANSFORM* get_current_projection_transform() const;

		bool clear_to_color(ALLEGRO_COLOR = al_map_rgb(0,0,0));

		bool set_blend_color(ALLEGRO_COLOR);
		bool set_blender(int op, int src, int dst);
		bool set_blender(int op, int src, int dst, int alpha_op, int alpha_src, int alpha_dst);
		bool get_blend_color(ALLEGRO_COLOR&);
		bool get_blender(int& op, int& src, int& dst);
		bool get_blender(int& op, int& src, int& dst, int& alpha_op, int& alpha_src, int& alpha_dst);
	};
}