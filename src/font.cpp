#include "../include/font.h"

namespace AllegroCPP {

	bool Font::_load_autotype(const std::string& pth, std::pair<int, int> res, int flg)
	{
		if (m_font) return false; // does not unload
		if (res.first == res.second) {
			if (pth.find(".ttf") != std::string::npos || pth.find(".otf") != std::string::npos)
			{
				m_font = al_load_ttf_font(pth.c_str(), res.first, flg);
				if (m_font) return true;
				m_font = al_load_font(pth.c_str(), res.first, flg);
				if (m_font) return true;
			}
			else {
				m_font = al_load_font(pth.c_str(), res.first, flg);
				if (m_font) return true;
				m_font = al_load_ttf_font(pth.c_str(), res.first, flg);
				if (m_font) return true;
			}
		}
		else {
			m_font = al_load_ttf_font_stretch(pth.c_str(), res.first, res.second, flg);
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

	Font::Font(int resolution, const std::string& path, int flags)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();
		
		if (!_load_autotype(path, { resolution, resolution }, flags)) throw std::runtime_error("Cannot load font");
	}

	Font::Font(std::pair<int, int> resolution, const std::string& ttfpath, int flags)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();

		if (!_load_autotype(ttfpath, resolution, flags)) throw std::runtime_error("Cannot load font");
	}

	Font::Font(int resolution, File&& ttffile, int flags)
	{
		if (ttffile.empty()) throw std::invalid_argument("File was null");
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();
		
		const std::string& fpnam = ttffile.get_filepath();
		if (fpnam.empty()) throw std::invalid_argument("File has no path!");

		// file.drop = drops handle
		if (!(m_font = al_load_ttf_font_f(((File_shareable_ptr&)ttffile)->release(), fpnam.c_str(), resolution, flags))) {
			throw std::runtime_error("Cannot load ttf font file");
		}
	}

	Font::Font(std::pair<int, int> resolution, File&& ttffile, int flags)
	{
		if (ttffile.empty()) throw std::invalid_argument("File was null");
		if (!al_is_system_installed()) al_init();
		if (!al_is_font_addon_initialized()) al_init_font_addon();
		if (!al_is_ttf_addon_initialized()) al_init_ttf_addon();
		
		const std::string& fpnam = ttffile.get_filepath();
		if (fpnam.empty()) throw std::invalid_argument("File has no path!");

		// file.drop = drops handle
		if (!(m_font = al_load_ttf_font_stretch_f(((File_shareable_ptr&)ttffile)->release(), fpnam.c_str(), resolution.first, resolution.second, flags))) {
			throw std::runtime_error("Cannot load ttf font file");
		}
	}

	Font::Font(ALLEGRO_BITMAP* glyphbmpautocopy, std::vector<font_ranges_pair> ranges)
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

	Font::Font(const std::string& glyphpath, std::vector<font_ranges_pair> ranges)
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

	Font::Font(const std::string& bmppath, int flags)
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
		: m_font(oth.m_font)
	{
		oth.m_font = nullptr;
	}

	void Font::operator=(Font&& oth) noexcept
	{
		if (m_font) al_destroy_font(m_font);
		m_font = oth.m_font;
		oth.m_font = nullptr;
	}

	bool Font::push_fallback(Font&& oth)
	{
		if (!m_font) return false;
		ALLEGRO_FONT* it = m_font;
		ALLEGRO_FONT* test = nullptr;
		while ((test = al_get_fallback_font(it))) it = test;
		al_set_fallback_font(m_font, oth.m_font);
		oth.m_font = nullptr;
		return true;
	}

	Font Font::pop_fallback()
	{
		if (!m_font) throw std::runtime_error("This font is not loaded");
		size_t falls = fallback_count();
		if (falls == 0) throw std::runtime_error("No fallback to pop!");
		ALLEGRO_FONT* it = m_font;
		for (size_t p = 1; p < falls; ++p) it = al_get_fallback_font(it);
		
		ALLEGRO_FONT* ret = al_get_fallback_font(it);
		al_set_fallback_font(it, nullptr);

		return Font(ret);
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

	bool Font::draw(std::pair<float, float> target, std::variant<std::string, const ALLEGRO_USTR*> text, ALLEGRO_COLOR color, text_alignment align, std::optional<font_delimiter_justified> just_settings)
	{
		if (!m_font) return false;

		ALLEGRO_USTR_INFO __ustr_auto{};
		const ALLEGRO_USTR* _str = (std::holds_alternative<std::string>(text) ? al_ref_cstr(&__ustr_auto, std::get<std::string>(text).c_str()) : std::get<const ALLEGRO_USTR*>(text));

		if (align == text_alignment::JUSTIFIED) { // JUST
			if (!just_settings.has_value()) return false; // needs just_settings!
			const auto& sett = just_settings.value();
			al_draw_justified_ustr(m_font, color, target.first, sett.max_x, target.second, sett.diff, sett.extra_flags, _str);
		}
		else { // LEFT, CENTER, RIGHT
			al_draw_ustr(m_font, color, target.first, target.second, static_cast<int>(align), _str);
		}

		return true;
	}

	//bool Font::drawf(std::pair<float, float> target, const std::string& text, ALLEGRO_COLOR color, text_alignment align, std::optional<font_delimiter_justified> just_settings, ...)
	//{
	//	if (!m_font) return false;
	//
	//	struct inline_destr_ustr { void operator()(ALLEGRO_USTR* b) { al_ustr_free(b); } };
	//	std::unique_ptr<ALLEGRO_USTR, inline_destr_ustr> buf(al_ustr_new(""), inline_destr_ustr());
	//
	//	va_list ap;
	//	va_start(ap, text.c_str());
	//	al_ustr_vappendf(buf.get(), text.c_str(), ap);
	//	va_end(ap);
	//
	//	if (align == text_alignment::JUSTIFIED) { // JUST
	//		if (!just_settings.has_value()) return false; // needs just_settings!
	//		const auto& sett = just_settings.value();
	//		al_draw_justified_ustr(m_font, color, target.first, sett.max_x, target.second, sett.diff, sett.extra_flags, buf.get());
	//	}
	//	else { // LEFT, CENTER, RIGHT
	//		al_draw_ustr(m_font, color, target.first, target.second, static_cast<int>(align), buf.get());
	//	}
	//
	//	return true;
	//}

	bool Font::draw_multiline(std::pair<float, float> target, std::variant<std::string, const ALLEGRO_USTR*> text, float max_width, float line_height, ALLEGRO_COLOR color, text_alignment align, std::optional<font_delimiter_justified> just_settings)
	{
		if (!m_font) return false;
		if (line_height < 0) line_height = static_cast<float>(get_line_height()) * 1.15f;
		if (max_width < 0) max_width = std::numeric_limits<float>::max();

		ALLEGRO_USTR_INFO __ustr_auto{};
		const ALLEGRO_USTR* _str = (std::holds_alternative<std::string>(text) ? al_ref_cstr(&__ustr_auto, std::get<std::string>(text).c_str()) : std::get<const ALLEGRO_USTR*>(text));

		__multiline_font_draw_ustr _ref{ m_font, color, line_height, target, align, just_settings };

		al_do_multiline_ustr(m_font, max_width, _str, &__do_text_ustr_multiline, &_ref);

		return true;
	}

	//bool Font::drawf_multiline(std::pair<float, float> target, const std::string& text, float max_width, float line_height, ALLEGRO_COLOR color, text_alignment align, std::optional<font_delimiter_justified> just_settings, ...)
	//{
	//	if (!m_font) return false;
	//
	//	struct inline_destr_ustr { void operator()(ALLEGRO_USTR* b) { al_ustr_free(b); } };
	//	std::unique_ptr<ALLEGRO_USTR, inline_destr_ustr> buf(al_ustr_new(""), inline_destr_ustr());
	//
	//	va_list ap;
	//	va_start(ap, text.c_str());
	//	al_ustr_vappendf(buf.get(), text.c_str(), ap);
	//	va_end(ap);
	//
	//	__multiline_font_draw_ustr _ref{ m_font, color, line_height, target, align, just_settings };
	//
	//	al_do_multiline_ustr(m_font, max_width, buf.get(), &__do_text_ustr_multiline, &_ref);
	//
	//	return true;
	//}

	bool Font::draw_glyph(std::pair<float, float> target, int codepoint, ALLEGRO_COLOR color)
	{
		if (!m_font) return false;

		al_draw_glyph(m_font, color, target.first, target.second, codepoint);
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
		const float y = s->target.second + (s->line_height * static_cast<float>(line_num));

		if (s->align == text_alignment::JUSTIFIED) { // JUST
			if (!s->delim_opt.has_value()) return false; // needs just_settings!
			const auto& sett = s->delim_opt.value();
			al_draw_justified_ustr(s->font, s->color, s->target.first, sett.max_x, y, sett.diff, sett.extra_flags, line);
		}
		else { // LEFT, CENTER, RIGHT
			al_draw_ustr(s->font, s->color, s->target.first, y, static_cast<int>(s->align), line);
		}

		return true;
	}


}