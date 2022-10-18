#pragma once

#include "../include/file.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include <optional>

#undef min
#undef max

namespace AllegroCPP {

	enum class text_alignment : int {LEFT = ALLEGRO_ALIGN_LEFT, CENTER = ALLEGRO_ALIGN_CENTER, RIGHT = ALLEGRO_ALIGN_RIGHT, JUSTIFIED};

	struct font_delimiter_justified {
		float max_x;
		float diff; // max space between words, 3+ words only
		int extra_flags = 0;
	};
	struct font_ustr_wrap {
		const ALLEGRO_USTR* str = nullptr;
	};

	struct font_text_dimensions {
		int x, y; // top left, may be negative
		int w, h; // dimensions
	};
	struct font_ranges_pair {
		int range[2]; // DO NOT CHANGE THIS
	};

	using font_prop = std::variant<ALLEGRO_COLOR, std::string, font_ustr_wrap, text_alignment, font_delimiter_justified>;

	class Font {
		ALLEGRO_FONT* m_font = nullptr;

		bool _load_autotype(const std::string& pth, std::pair<int, int> res, int flg);

		void _clean_fonts(); // include fallback popping
		std::vector<ALLEGRO_FONT*> _list_fonts(); // from this to next fallback until null (has m_font)

		Font(ALLEGRO_FONT* eat);
	public:
		Font(int resolution, const std::string& path, int flags = 0);
		Font(std::pair<int,int> resolution, const std::string& ttfpath, int flags = 0);
		Font(int resolution, File&& ttffile, int flags = 0);
		Font(std::pair<int, int> resolution, File&& ttffile, int flags = 0);
		Font(ALLEGRO_BITMAP* glyphbmpautocopy, std::vector<font_ranges_pair> ranges);
		Font(const std::string& glyphpath, std::vector<font_ranges_pair> ranges);
		Font(const std::string& bmppath, int flags = 0);
		Font();
		~Font();

		bool empty() const;
		bool valid() const;
		operator bool() const;
		operator ALLEGRO_FONT* ();

		Font(const Font&) = delete;
		Font(Font&&) noexcept;
		void operator=(const Font&) = delete;
		void operator=(Font&&) noexcept;

		bool push_fallback(Font&&);
		Font pop_fallback();
		size_t fallback_count();

		int get_line_height() const;
		int get_ascent() const;
		int get_descent() const;
		int get_width(const char*);
		int get_width(const std::string&);
		int get_width(ALLEGRO_USTR const*);
		int get_glyph_width(int codepoint);
		font_text_dimensions get_dimensions(const char*);
		font_text_dimensions get_dimensions(const std::string&);
		font_text_dimensions get_dimensions(ALLEGRO_USTR const*);
		font_text_dimensions get_glyph_dimensions(int codepoint);
		int get_glypth_advance(int codepoint, int codepoint2 = ALLEGRO_NO_KERNING);

		bool draw(std::pair<float, float> target, std::variant<std::string, const ALLEGRO_USTR*> text, ALLEGRO_COLOR color = al_map_rgb(255,255,255), text_alignment align = text_alignment::LEFT, std::optional<font_delimiter_justified> just_settings = {});
		//bool drawf(std::pair<float, float> target, const std::string& text, ALLEGRO_COLOR color, text_alignment align, std::optional<font_delimiter_justified> just_settings, ...);
		bool draw_multiline(std::pair<float, float> target, std::variant<std::string, const ALLEGRO_USTR*> text, float max_width = -1.f, float line_height = -1.f, ALLEGRO_COLOR color = al_map_rgb(255, 255, 255), text_alignment align = text_alignment::LEFT, std::optional<font_delimiter_justified> just_settings = {});
		//bool drawf_multiline(std::pair<float, float> target, const std::string& text, float max_width, float line_height, ALLEGRO_COLOR color, text_alignment align, std::optional<font_delimiter_justified> just_settings, ...);

		bool draw_glyph(std::pair<float, float> target, int codepoint, ALLEGRO_COLOR color = al_map_rgb(255, 255, 255));

		std::vector<font_ranges_pair> get_ranges(int max = -1);
	};

	struct __multiline_font_draw_ustr {
		const ALLEGRO_FONT* font;
		const ALLEGRO_COLOR& color;
		const float& line_height;
		const std::pair<float, float>& target;
		const text_alignment& align;
		std::optional<font_delimiter_justified> delim_opt;
	};

	bool __do_text_ustr_multiline(int line_num, const ALLEGRO_USTR* line, void* extra);

}