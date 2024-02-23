#pragma once

#include "../include/file.h"
#include "../include/utfstring.h"

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
		float max_x = 0.0f;
		float diff = 0.0f; // max space between words, 3+ words only
		int extra_flags = 0;
	};
	struct font_text_dimensions {
		int x, y; // top left, may be negative
		int w, h; // dimensions
	};
	struct font_ranges_pair {
		int range[2]; // DO NOT CHANGE THIS
	};
	struct font_multiline_props {
		float max_width = -1.0f;
		float line_height = -1.0f;
	};
	struct font_position {
		float target_x, target_y;
	};
	enum class font_multiline_b {NO_MULTILINE, MULTILINE};

	using font_prop = std::variant<ALLEGRO_COLOR, std::string, UTFstring, text_alignment, font_delimiter_justified, font_multiline_b, font_multiline_props, font_position>;

	class Font {
	public:
		std::shared_ptr<ALLEGRO_FONT> m_font;

		struct draw_props {
			UTFstring _string;
			ALLEGRO_COLOR _color = al_map_rgb(255, 255, 255);
			text_alignment _align = text_alignment::LEFT;
			font_delimiter_justified _justified_props = {};
			font_multiline_props _multiline_props = {};
			font_position _pos = {};
			bool _multiline = false;

			void reset();
		};
		draw_props m_stored_draw_props;

		bool _load_autotype(const std::string& pth, const int res_x, const int res_y, int flg);

		//void _clean_fonts(); // include fallback popping
		//std::vector<ALLEGRO_FONT*> _list_fonts(); // from this to next fallback until null (has m_font)

		Font(ALLEGRO_FONT* eat);
		Font(std::shared_ptr<ALLEGRO_FONT> dup, draw_props prop);
	public:
		Font(const int resolution, const std::string& path, const int flags = 0);
		Font(const int resolution_x, const int resolution_y, const std::string& ttfpath, const int flags = 0);
		Font(const int resolution, File&& ttffile, const int flags = 0);
		Font(const int resolution_x, const int resolution_y, File&& ttffile, const int flags = 0);
		Font(ALLEGRO_BITMAP* glyphbmpautocopy, const std::vector<font_ranges_pair> ranges);
		Font(const std::string& glyphpath, const std::vector<font_ranges_pair> ranges);
		Font(const std::string& bmppath, const int flags = 0);
		Font();

		virtual bool empty() const;
		virtual bool valid() const;
		virtual operator bool() const;
		virtual operator ALLEGRO_FONT* ();

		Font(const Font&) = delete;
		Font(Font&&) noexcept;
		void operator=(const Font&) = delete;
		void operator=(Font&&) noexcept;

		virtual void destroy();
		virtual Font make_ref() const;

		virtual bool push_fallback(Font&&);
		virtual Font pop_fallback();
		virtual size_t fallback_count() const;

		virtual int get_line_height() const;
		virtual int get_ascent() const;
		virtual int get_descent() const;
		virtual int get_width(const char*);
		virtual int get_width(const std::string&);
		virtual int get_width(ALLEGRO_USTR const*);
		virtual int get_glyph_width(int codepoint);
		virtual font_text_dimensions get_dimensions(const char*);
		virtual font_text_dimensions get_dimensions(const std::string&);
		virtual font_text_dimensions get_dimensions(ALLEGRO_USTR const*);
		virtual font_text_dimensions get_glyph_dimensions(int codepoint);
		virtual int get_glypth_advance(int codepoint, int codepoint2 = ALLEGRO_NO_KERNING);

		virtual void set_draw_property(font_prop prop);
		virtual void set_draw_properties(std::vector<font_prop> props);
		virtual void reset_draw_properties();

		virtual bool draw() const;
		virtual bool draw(float target_x, float target_y, const UTFstring&);
		virtual bool draw(float target_x, float target_y);

		virtual bool draw_glyph(float target_x, float target_y, int codepoint, ALLEGRO_COLOR color = al_map_rgb(255, 255, 255));

		virtual std::vector<font_ranges_pair> get_ranges(int max = -1);
	};

	struct __multiline_font_draw_ustr {
		const std::shared_ptr<ALLEGRO_FONT> font;
		const ALLEGRO_COLOR& color;
		const float& line_height;
		const float& target_x;
		const float& target_y;
		const text_alignment& align;
		std::optional<font_delimiter_justified> delim_opt;
	};

	bool __do_text_ustr_multiline(int line_num, const ALLEGRO_USTR* line, void* extra);

}