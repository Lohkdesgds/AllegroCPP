#include "display.h"

#ifdef _WIN32
#include <windows.h>
#include <Richedit.h>
#endif

#include <unordered_map>
#include <utility>
#include <algorithm>

namespace AllegroCPP {

	Display::Display(const int size_x, const int size_y, const std::string& windowname, const int flags, const int pos_x, const int pos_y, const int refresh_rate, const std::vector<display_option> options, const bool centered_on_pos_x, const bool centered_on_pos_y)
	{
		if (!create(size_x, size_y, windowname, flags, pos_x, pos_y, refresh_rate, options, centered_on_pos_x, centered_on_pos_y)) throw std::runtime_error("Failed creating display!");
	}

	Display::~Display()
	{
		destroy();
	}

	Display::Display(Display&& oth) noexcept
		: m_disp(oth.m_disp)
	{
		oth.m_disp.reset();
	}

	void Display::operator=(Display&& oth) noexcept
	{
		m_disp = oth.m_disp;
		oth.m_disp.reset();
	}

	bool Display::create(const int size_x, const int size_y, const std::string& windowname, const int flags, const int pos_x, const int pos_y, const int refresh_rate, const std::vector<display_option> options, const bool centered_on_pos_x, const bool centered_on_pos_y)
	{
		if (!al_is_system_installed())
			al_init();

		if (m_disp)
			destroy();

		if (flags >= 0) {
			al_set_new_display_flags(flags);
		}

		if (pos_x != display_undefined_position[0] && pos_y != display_undefined_position[1]) {
			al_set_new_window_position(centered_on_pos_x ? (pos_x - 0.5f * size_x) : pos_x, centered_on_pos_y ? (pos_y - 0.5f * size_y) : pos_y);
		}

		if (refresh_rate > 0) {
			al_set_new_display_refresh_rate(refresh_rate);
		}

		for (const auto& i : options) {
			al_set_new_display_option(i.option, i.value, i.importance);
		}

		const auto new_windowname = windowname.substr(0, ALLEGRO_NEW_WINDOW_TITLE_MAX_SIZE);

		al_set_new_window_title(new_windowname.c_str());

		auto new_display = al_create_display(size_x, size_y);

		return (m_disp = std::shared_ptr<ALLEGRO_DISPLAY>(new_display,
			[](ALLEGRO_DISPLAY* d) {al_set_display_menu(d, nullptr); al_destroy_display(d); })
		).get();
	}

	void Display::destroy()
	{
		m_disp.reset();
	}

	bool Display::empty() const
	{
		return m_disp.get() == nullptr;
	}

	bool Display::valid() const
	{
		return m_disp.get() != nullptr;
	}

	Display::operator bool() const
	{
		return valid();
	}

	Display::operator ALLEGRO_DISPLAY* ()
	{
		return m_disp.get();
	}

	Display::operator ALLEGRO_EVENT_SOURCE* ()
	{
		return m_disp ? al_get_display_event_source(m_disp.get()) : nullptr;
	}

	Display::operator std::weak_ptr<ALLEGRO_DISPLAY> ()
	{
		return m_disp;
	}

	ALLEGRO_EVENT_SOURCE* Display::get_event_source() const
	{
		return m_disp ? al_get_display_event_source(m_disp.get()) : nullptr;
	}

	ALLEGRO_BITMAP* Display::get_backbuffer()
	{
		return m_disp ? al_get_backbuffer(m_disp.get()) : nullptr;
	}

	ALLEGRO_DISPLAY* Display::get_display()
	{
		return m_disp.get();
	}

	std::weak_ptr<ALLEGRO_DISPLAY> Display::get_display_ref()
	{
		return m_disp;
	}

	bool Display::flip()
	{
		if (!m_disp) return false;
		if (al_get_current_display() != m_disp.get()) al_set_target_backbuffer(m_disp.get());
		al_flip_display();
		return true;
	}

	bool Display::update_region(const int pos_x, const int pos_y, const int width, const int height)
	{
		if (!m_disp) return false;
		if (al_get_current_display() != m_disp.get()) al_set_target_backbuffer(m_disp.get());
		al_update_display_region(pos_x, pos_y, width, height);
		return true;
	}

	void Display::wait_for_vsync() const
	{
		if (!m_disp) return;
		if (al_get_current_display() != m_disp.get()) al_set_target_backbuffer(m_disp.get());
		al_wait_for_vsync();
	}

	int Display::get_width() const
	{
		return m_disp ? al_get_display_width(m_disp.get()) : -1;
	}

	int Display::get_height() const
	{
		return m_disp ? al_get_display_height(m_disp.get()) : -1;
	}

	bool Display::resize(const int width, const int height)
	{
		if (!m_disp) return false;
		if (!al_resize_display(m_disp.get(), width, height)) return false;
		return true;
	}

	void Display::acknowledge_resize()
	{
		if (m_disp) al_acknowledge_resize(m_disp.get());
	}

	bool Display::get_position(int& x, int& y) const
	{
		if (!m_disp) return false;
		al_get_window_position(m_disp.get(), &x, &y);
		return true;
	}

	bool Display::set_position(const int pos_x, const int pos_y)
	{
		if (!m_disp) return false;
		al_set_window_position(m_disp.get(), pos_x, pos_y);
		return true;
	}

	bool Display::get_constraints(int& min_x, int& min_y, int& max_x, int& max_y) const
	{
		if (!m_disp) return false;
		al_get_window_constraints(m_disp.get(), &min_x, &min_y, &max_x, &max_y);
		return true;
	}

	bool Display::set_constraints(const int min_x, const int min_y, const int max_x, const int max_y)
	{
		if (!m_disp) return false;
		al_set_window_constraints(m_disp.get(), min_x, min_y, max_x, max_y);
		return true;
	}

	bool Display::apply_constraints(const bool enab)
	{
		if (!m_disp) return false;
		al_apply_window_constraints(m_disp.get(), enab);
		return true;
	}

	int Display::get_flags() const
	{
		return m_disp ? al_get_display_flags(m_disp.get()) : -1;
	}

	bool Display::set_flag(const int flag, const bool enabled)
	{
		if (!m_disp) return false;
		return al_set_display_flag(m_disp.get(), flag, enabled);
	}

	int Display::get_option(const int opt) const
	{
		return m_disp ? al_get_display_option(m_disp.get(), opt) : -1;
	}

	bool Display::set_option(const int opt, const int val)
	{
		if (!m_disp.get()) return false;
		al_set_display_option(m_disp.get(), opt, val);
		return true;
	}

	int Display::get_format() const
	{
		return m_disp ? al_get_display_format(m_disp.get()) : -1;
	}

	int Display::get_orientation() const
	{
		return m_disp ? al_get_display_orientation(m_disp.get()) : -1;
	}

	int Display::get_refresh_rate() const
	{
		return m_disp ? al_get_display_refresh_rate(m_disp.get()) : -1;
	}

	bool Display::set_title(const std::string& title)
	{
		if (!m_disp) return false;
		const auto new_title = title.substr(0, ALLEGRO_NEW_WINDOW_TITLE_MAX_SIZE);
		al_set_window_title(m_disp.get(), new_title.c_str());
		return true;
	}

	bool Display::set_icon(ALLEGRO_BITMAP* bitmap)
	{
		if (!m_disp || !bitmap) return false;
		al_set_display_icon(m_disp.get(), bitmap);
		return true;
	}

	bool Display::set_icons(std::vector<ALLEGRO_BITMAP*> vec)
	{
		if (!m_disp || vec.size() == 0 || std::find(vec.begin(), vec.end(), nullptr) != vec.end()) return false;
		al_set_display_icons(m_disp.get(), static_cast<int>(vec.size()), vec.data());
		return true;
	}

	bool Display::acknowledge_drawing_halt()
	{
		if (!m_disp) return false;
		al_acknowledge_drawing_halt(m_disp.get());
		return true;
	}

	bool Display::acknowledge_drawing_resume()
	{
		if (!m_disp) return false;
		al_acknowledge_drawing_resume(m_disp.get());
		return true;
	}

	void Display::inhibit_screensaver(const bool inhibit)
	{
		al_inhibit_screensaver(inhibit);
	}

	bool Display::get_has_clipboard_text() const
	{
		if (!m_disp) return false;
		return al_clipboard_has_text(m_disp.get());
	}

	std::string Display::get_clipboard_text() const
	{
		if (!m_disp) return {};
		const char* str = al_clipboard_has_text(m_disp.get()) ? al_get_clipboard_text(m_disp.get()) : nullptr;
		return str ? str : std::string{};
	}

	bool Display::clear_clipboard_text()
	{
		if (!m_disp) return false;
		return al_set_clipboard_text(m_disp.get(), nullptr);
	}

	bool Display::set_clipboard_text(const std::string& str)
	{
		if (!m_disp) return false;
		return al_set_clipboard_text(m_disp.get(), str.c_str());
	}

	bool Display::set_as_target()
	{
		if (!m_disp) return false;
		al_set_target_backbuffer(m_disp.get());
		return true;
	}

	bool Display::set_clip_rectangle(const int pos_x, const int pos_y, const int width, const int height)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_set_clipping_rectangle(pos_x, pos_y, width, height);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::get_clip_rectangle(int& pos_x, int& pos_y, int& width, int& height) const
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_get_clipping_rectangle(&pos_x, &pos_y, &width, &height);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::reset_clip_rectangle()
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_reset_clipping_rectangle();
		al_set_target_bitmap(oldtarg);
		return true;
	}

	const ALLEGRO_TRANSFORM* Display::get_current_transform() const
	{
		if (!m_disp) return nullptr;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		const ALLEGRO_TRANSFORM* currt = al_get_current_transform();
		al_set_target_bitmap(oldtarg);
		return currt;
	}

	const ALLEGRO_TRANSFORM* Display::get_current_inverse_transform() const
	{
		if (!m_disp) return nullptr;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		const ALLEGRO_TRANSFORM* currt = al_get_current_inverse_transform();
		al_set_target_bitmap(oldtarg);
		return currt;
	}

	const ALLEGRO_TRANSFORM* Display::get_current_projection_transform() const
	{
		if (!m_disp) return nullptr;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		const ALLEGRO_TRANSFORM* currt = al_get_current_projection_transform();
		al_set_target_bitmap(oldtarg);
		return currt;
	}

	bool Display::clear_to_color(const ALLEGRO_COLOR color)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_clear_to_color(color);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::set_blend_color(const ALLEGRO_COLOR color)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_set_blend_color(color);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::set_blender(const int op, const int src, const int dst)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_set_blender(op, src, dst);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::set_blender(const int op, const int src, const int dst, const int alpha_op, const int alpha_src, const int alpha_dst)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_set_separate_blender(op, src, dst, alpha_op, alpha_src, alpha_dst);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::get_blend_color(ALLEGRO_COLOR& color)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		color = al_get_blend_color();
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::get_blender(int& op, int& src, int& dst)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_get_blender(&op, &src, &dst);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Display::get_blender(int& op, int& src, int& dst, int& alpha_op, int& alpha_src, int& alpha_dst)
	{
		if (!m_disp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_backbuffer(m_disp.get());
		al_get_separate_blender(&op, &src, &dst, &alpha_op, &alpha_src, &alpha_dst);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	void Display::operator<<(ALLEGRO_MENU* m)
	{
		if (m_disp && m) al_set_display_menu(m_disp.get(), m);
	}

#ifdef _WIN32
	bool Display::set_icon_from_resource(const int id)
	{
		HICON icon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(id), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
		if (icon) {
			HWND winhandle = al_get_win_window_handle(m_disp.get());
			SetClassLongPtr(winhandle, GCLP_HICON, (LONG_PTR)icon);
			SetClassLongPtr(winhandle, GCLP_HICONSM, (LONG_PTR)icon);
			DestroyIcon(icon);
			return true;
		}
		return false;
	}

	HWND Display::get_window_handler()
	{
		return m_disp.get() ? al_get_win_window_handle(m_disp.get()) : nullptr;
	}

	/* source of functions here (adapted): https://stackoverflow.com/questions/45971548/setlayeredwindowattributes-not-working-on-windows-8-c */

	//void display_error(DWORD error)
	//{
	//	char buf[100];
	//	wsprintfA(buf, "Error %u!", error);
	//	MessageBoxA(NULL, buf, 0, 0); // Ideally you would pass a window handle here but I don't know if your handle is actually valid
	//}

	bool window_fix_transparent_color(HWND window_handle, bool enable)
	{
		DWORD error;

		// get the window flags to see if RGB color transparency is supported.
		SetLastError(0);
		LONG_PTR ExStyle = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
		if (ExStyle == 0)
		{
			error = GetLastError();
			if (error != 0) return false;
		}

		if ((ExStyle & WS_EX_LAYERED) == 0)
		{
			if (!enable) return true;

			// set the window flags to support RGB color transparency.
			SetLastError(0);
			if (!SetWindowLongPtr(window_handle, GWL_EXSTYLE, ExStyle | WS_EX_LAYERED))
			{
				error = GetLastError();
				if (error != 0) return false;
			}
		}
		else {
			if (enable) return true;

			// unset the window flags to support RGB color transparency.
			SetLastError(0);
			if (!SetWindowLongPtr(window_handle, GWL_EXSTYLE, ExStyle & (~WS_EX_LAYERED)))
			{
				error = GetLastError();
				if (error != 0) return false;
			}
		}

		return true;
	}

	bool window_set_transparent_color(HWND window_handle, BYTE red, BYTE green, BYTE blue)
	{
		// sets the RGB color to be transparent for the specified window.
		return SetLayeredWindowAttributes(window_handle, RGB(red, green, blue), 255, LWA_COLORKEY);
	}



	bool Display::make_window_masked(ALLEGRO_COLOR mask)
	{
		auto hwnd = get_window_handler();

		return window_fix_transparent_color(hwnd, true) &&
			window_set_transparent_color(hwnd, static_cast<BYTE>(mask.r * 255.0f), static_cast<BYTE>(mask.g * 255.0f), static_cast<BYTE>(mask.b * 255.0f));
	}

	bool Display::unmake_window_masked()
	{
		auto hwnd = get_window_handler();

		return window_fix_transparent_color(hwnd, false);
	}
#endif

}