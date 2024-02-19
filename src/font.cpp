#include "../include/font.h"

namespace AllegroCPP {

	void Font::draw_props::reset()
	{
		*this = {};
	}

	bool Font::_load_autotype(const std::string& pth, const int res_x, const int res_y, int flg)
	{
		if (m_font) return false; // does not unload
		if (res_x == res_y) {
			if (pth.find(".ttf") != std::string::npos || pth.find(".otf") != std::string::npos)
			{
				m_font = al_load_ttf_font(pth.c_str(), res_x, flg);
				if (m_font) return true;
				m_font = al_load_font(pth.c_str(), res_x, flg);
				if (m_font) return true;
			}
			else {
				m_font = al_load_font(pth.c_str(), res_x, flg);
				if (m_font) return true;
				m_font = al_load_ttf_font(pth.c_str(), res_x, flg);
				if (m_font) return true;
			}
		}
		else {
			m_font = al_load_ttf_font_stretch(pth.c_str(), res_x, res_y, flg);
			if (m_font) return true;
		}
		return false;
	}

	void Font::_clean_fonts()
	{
		// has m_font there
		for(auto* ff : _list_fonts()) al_destroy_font(ff);
		m_font = nullptr;
	}

	std::vector<ALLEGRO_FONT*> Font::_list_fonts()
	{
		if (!m_font) return {};
		std::vector<ALLEGRO_FONT*> vc;
		vc.push_back(m_font);
		for (ALLEGRO_FONT* currdown = al_get_fallback_font(m_font); currdown != nullptr; currdown = al_get_fallback_font(m_font))
			vc.push_back(currdown);

		return vc;
	}

	Font::Font(ALLEGRO_FONT* eat)
		: m_font(eat)
	{
	}

	Font::Font(const int resolution, const std::string& path, const int flags)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();
		
		if (!_load_autotype(path, resolution, resolution, flags)) throw std::runtime_error("Cannot load font");
	}

	Font::Font(const int resolution_x, const int resolution_y, const std::string& ttfpath, const int flags)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();

		if (!_load_autotype(ttfpath, resolution_x, resolution_y, flags)) throw std::runtime_error("Cannot load font");
	}

	Font::Font(const int resolution, File&& ttffile, const int flags)
	{
		if (ttffile.empty()) throw std::invalid_argument("File was null");
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();
		
		const std::string& fpnam = ttffile.get_filepath();
		if (fpnam.empty()) throw std::invalid_argument("File has no path!");

		File_shareable_ptr drop = ttffile;
		if (!(m_font = al_load_ttf_font_f(drop->release(), fpnam.c_str(), resolution, flags))) {
			throw std::runtime_error("Cannot load ttf font file");
		}
	}

	Font::Font(const int resolution_x, const int resolution_y, File&& ttffile, const int flags)
	{
		if (ttffile.empty()) throw std::invalid_argument("File was null");
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();
		
		const std::string& fpnam = ttffile.get_filepath();
		if (fpnam.empty()) throw std::invalid_argument("File has no path!");

		File_shareable_ptr drop = ttffile;
		if (!(m_font = al_load_ttf_font_stretch_f(drop->release(), fpnam.c_str(), resolution_x, resolution_y, flags))) {
			throw std::runtime_error("Cannot load ttf font file");
		}
	}

	Font::Font(ALLEGRO_BITMAP* glyphbmpautocopy, const std::vector<font_ranges_pair> ranges)
	{
		if (!glyphbmpautocopy || ranges.empty()) throw std::invalid_argument("Bitmap or ranges empty");
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();

		// this copy bmp, it's safe
		if (!(m_font = al_grab_font_from_bitmap(glyphbmpautocopy, static_cast<int>(ranges.size()), (int*)ranges.data()))) {
			throw std::runtime_error("Cannot load glyph font");
		}
	}

	Font::Font(const std::string& glyphpath, const std::vector<font_ranges_pair> ranges)
	{
		if (glyphpath.empty() || ranges.empty()) throw std::invalid_argument("Path or ranges empty");
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();

		struct inline_destr_bmp{ void operator()(ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); } };
		auto bbmp = std::unique_ptr<ALLEGRO_BITMAP, inline_destr_bmp> (al_load_bitmap(glyphpath.c_str()), inline_destr_bmp());
		if (bbmp.get() == nullptr) throw std::runtime_error("Cannot load bitmap glyph for font");

		// this copy bmp, it's safe
		if (!(m_font = al_grab_font_from_bitmap(bbmp.get(), static_cast<int>(ranges.size()), (int*)ranges.data()))) {
			throw std::runtime_error("Cannot load glyph font");
		}
	}

	Font::Font(const std::string& bmppath, const int flags)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();

		if (!(m_font = al_load_bitmap_font_flags(bmppath.c_str(), flags))) {
			throw std::runtime_error("Cannot load bitmap font");
		}
	}

	Font::Font()
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();

		if (!(m_font = al_create_builtin_font())) {
			throw std::runtime_error("Cannot load builtin font");
		}
	}

	Font::~Font()
	{
		_clean_fonts();
	}

	bool Font::empty() const
	{
		return m_font == nullptr;
	}

	bool Font::valid() const
	{
		return m_font != nullptr;
	}

	Font::operator bool() const
	{
		return m_font != nullptr;
	}

	Font::operator ALLEGRO_FONT* ()
	{
		return m_font;
	}

	Font::Font(Font&& oth) noexcept
		: m_font(oth.m_font), m_stored_draw_props(oth.m_stored_draw_props)
	{
		oth.m_font = nullptr;
		oth.m_stored_draw_props.reset();
	}

	void Font::operator=(Font&& oth) noexcept
	{
		if (m_font) al_destroy_font(m_font);
		m_font = oth.m_font;
		m_stored_draw_props = oth.m_stored_draw_props;
		oth.m_font = nullptr;
		oth.m_stored_draw_props.reset();
	}

	bool Font::push_fallback(Font&& oth)
	{
		if (!m_font) return false;
		ALLEGRO_FONT* it = m_font;
		ALLEGRO_FONT* test = nullptr;
		while ((test = al_get_fallback_font(it))) it = test;
		al_set_fallback_font(it, oth.m_font);
		oth.m_font = nullptr;
		return true;
	}

	Font Font::pop_fallback()
	{
		if (!m_font) throw std::runtime_error("This font is not loaded");
		ALLEGRO_FONT* it = m_font;
		ALLEGRO_FONT* test = nullptr;
		while ((test = al_get_fallback_font(it)) != nullptr) {
			if (al_get_fallback_font(test) == nullptr) { // it --fb--> test --fb--> == nullptr?
				al_set_fallback_font(it, nullptr);
				return Font(test);
			}
		}

		return Font(nullptr);
	}

	size_t Font::fallback_count()
	{
		if (!m_font) return 0;
		size_t c = 0;
		ALLEGRO_FONT* it = m_font;
		while ((it = al_get_fallback_font(it)) != nullptr) ++c;
		return c;
	}

	int Font::get_line_height() const
	{
		return m_font ? al_get_font_line_height(m_font) : -1;
	}

	int Font::get_ascent() const
	{
		return m_font ? al_get_font_ascent(m_font) : -1;
	}

	int Font::get_descent() const
	{
		return m_font ? al_get_font_descent(m_font) : -1;
	}

	int Font::get_width(const char* str)
	{
		return m_font ? al_get_text_width(m_font, str) : -1;
	}

	int Font::get_width(const std::string& str)
	{
		return m_font ? al_get_text_width(m_font, str.c_str()) : -1;
	}

	int Font::get_width(ALLEGRO_USTR const* str)
	{
		return m_font ? al_get_ustr_width(m_font, str) : -1;
	}

	int Font::get_glyph_width(int codepoint)
	{
		return m_font ? al_get_glyph_width(m_font, codepoint) : -1;
	}

	void Font::set_draw_property(font_prop prop)
	{
		if (std::holds_alternative<ALLEGRO_COLOR>(prop))					m_stored_draw_props._color = std::get<ALLEGRO_COLOR>(prop);
		else if (std::holds_alternative<std::string>(prop))					m_stored_draw_props._string = std::get<std::string>(prop);
		else if (std::holds_alternative<UTFstring>(prop))					m_stored_draw_props._string = std::get<UTFstring>(prop);
		else if (std::holds_alternative<text_alignment>(prop))				m_stored_draw_props._align = std::get<text_alignment>(prop);
		else if (std::holds_alternative<font_delimiter_justified>(prop))	m_stored_draw_props._justified_props = std::get<font_delimiter_justified>(prop);
		else if (std::holds_alternative<font_multiline_b>(prop))			m_stored_draw_props._multiline = (std::get<font_multiline_b>(prop) == font_multiline_b::MULTILINE);
		else if (std::holds_alternative<font_multiline_props>(prop))		m_stored_draw_props._multiline_props = std::get<font_multiline_props>(prop);
		else if (std::holds_alternative<font_position>(prop))		m_stored_draw_props._pos = std::get<font_position>(prop);
	}

	void Font::set_draw_properties(std::vector<font_prop> props)
	{
		for (const auto& i : props) set_draw_property(i);
	}
	
	void Font::reset_draw_properties()
	{
		m_stored_draw_props.reset();
	}

	font_text_dimensions Font::get_dimensions(const char* str)
	{
		if (!m_font) return { -1,-1,-1,-1 };
		font_text_dimensions fd{};
		al_get_text_dimensions(m_font, str, &fd.x, &fd.y, &fd.w, &fd.h);
		return fd;
	}

	font_text_dimensions Font::get_dimensions(const std::string& str)
	{
		if (!m_font) return { -1,-1,-1,-1 };
		font_text_dimensions fd{};
		al_get_text_dimensions(m_font, str.c_str(), &fd.x, &fd.y, &fd.w, &fd.h);
		return fd;
	}

	font_text_dimensions Font::get_dimensions(ALLEGRO_USTR const* str)
	{
		if (!m_font) return { -1,-1,-1,-1 };
		font_text_dimensions fd{};
		al_get_ustr_dimensions(m_font, str, &fd.x, &fd.y, &fd.w, &fd.h);
		return fd;
	}

	font_text_dimensions Font::get_glyph_dimensions(int codepoint)
	{
		if (!m_font) return { -1,-1,-1,-1 };
		font_text_dimensions fd{};
		al_get_glyph_dimensions(m_font, codepoint, &fd.x, &fd.y, &fd.w, &fd.h);
		return fd;
	}

	int Font::get_glypth_advance(int codepoint, int codepoint2)
	{
		return m_font ? al_get_glyph_advance(m_font, codepoint, codepoint2) : 0;
	}

	bool Font::draw() const
	{
		if (!m_font) return false;

		ALLEGRO_USTR_INFO __ustr_auto{};
		const ALLEGRO_USTR* _str = m_stored_draw_props._string;

		if (_str == nullptr) return false;

		if (m_stored_draw_props._multiline) {
			const float max_width =
				m_stored_draw_props._multiline_props.max_width < 0.0f ?
				std::numeric_limits<float>::max() :
				m_stored_draw_props._multiline_props.max_width;

			const float line_height =
				m_stored_draw_props._multiline_props.line_height < 0.0f ?
				static_cast<float>(get_line_height()) * 1.15f :
				m_stored_draw_props._multiline_props.line_height;


			__multiline_font_draw_ustr _ref{ m_font, m_stored_draw_props._color, line_height, m_stored_draw_props._pos.target_x, m_stored_draw_props._pos.target_y, m_stored_draw_props._align, m_stored_draw_props._justified_props };
			al_do_multiline_ustr(m_font, max_width, _str, &__do_text_ustr_multiline, &_ref);
		}
		else {
			if (m_stored_draw_props._align == text_alignment::JUSTIFIED)
			{
				const auto& sett = m_stored_draw_props._justified_props;
				al_draw_justified_ustr(m_font, m_stored_draw_props._color, m_stored_draw_props._pos.target_x, sett.max_x, m_stored_draw_props._pos.target_y, sett.diff, sett.extra_flags, _str);
			}
			else
			{
				al_draw_ustr(m_font, m_stored_draw_props._color, m_stored_draw_props._pos.target_x, m_stored_draw_props._pos.target_y, static_cast<int>(m_stored_draw_props._align), _str);
			}
		}

		return true;
	}

	bool Font::draw(float target_x, float target_y, const UTFstring& str)
	{
		m_stored_draw_props._string = str;
		m_stored_draw_props._pos.target_x = target_x;
		m_stored_draw_props._pos.target_y = target_y;
		return draw();
	}

	bool Font::draw(float target_x, float target_y)
	{
		m_stored_draw_props._pos.target_x = target_x;
		m_stored_draw_props._pos.target_y = target_y;
		return draw();
	}


	bool Font::draw_glyph(float target_x, float target_y, int codepoint, ALLEGRO_COLOR color)
	{
		if (!m_font) return false;

		al_draw_glyph(m_font, color, target_x, target_y, codepoint);
		return true;
	}

	std::vector<font_ranges_pair> Font::get_ranges(int max)
	{
		int totalranges = max < 0 ? al_get_font_ranges(m_font, 0, nullptr) : max;
		if (totalranges <= 0) return {};
		std::vector<font_ranges_pair> vec(static_cast<size_t>(max), {0,0});

		al_get_font_ranges(m_font, static_cast<int>(vec.size()), (int*)vec.data());
		return vec;
	}



	bool __do_text_ustr_multiline(int line_num, const ALLEGRO_USTR* line, void* extra)
	{
		__multiline_font_draw_ustr* s = (__multiline_font_draw_ustr*)extra;
		const float y = s->target_y + (s->line_height * static_cast<float>(line_num));

		if (s->align == text_alignment::JUSTIFIED) { // JUST
			if (!s->delim_opt.has_value()) return false; // needs just_settings!
			const auto& sett = s->delim_opt.value();
			al_draw_justified_ustr(s->font, s->color, s->target_x, sett.max_x, y, sett.diff, sett.extra_flags, line);
		}
		else { // LEFT, CENTER, RIGHT
			al_draw_ustr(s->font, s->color, s->target_x, y, static_cast<int>(s->align), line);
		}

		return true;
	}


}