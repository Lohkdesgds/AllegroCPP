#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include "bitmap.h"

#include <memory>
#include <stdexcept>

namespace AllegroCPP {

	// algif.h, algif.cpp, bitmap.cpp, gif.cpp, lzw.cpp

	class GIF : protected Bitmap {
		struct ALGIF_RGB { uint8_t r{}, g{}, b{}; };
		struct ALGIF_PALETTE { int colors_count{}; ALGIF_RGB colors[256]{}; };
        struct ALGIF_BITMAP { int w{}, h{}; uint8_t* data = nullptr; };
        struct ALGIF_FRAME {
            ALGIF_BITMAP* bitmap_8_bit{};
            ALGIF_PALETTE palette{};
            int xoff{}, yoff{};
            int duration{};               /* in 1/100th seconds */
            int disposal_method{};        /* 0 = don't care, 1 = keep, 2 = background, 3 = previous */
            int transparent_index{};

            ALLEGRO_BITMAP* rendered = nullptr;
        };
        struct ALGIF_ANIMATION {
            int width{}, height{};
            int frames_count{};
            int background_index{};
            int loop{}; /* -1 = no, 0 = forever, 1..65535 = that many times */
            ALGIF_PALETTE palette{};
            ALGIF_FRAME* frames{};

            int duration{};
            ALLEGRO_BITMAP* store{};
        };

        ALGIF_ANIMATION* m_anim = nullptr;
		double m_start_time{};

        int read_code(ALLEGRO_FILE* const&, char*, int*, int);
        int LZW_decode(ALLEGRO_FILE* const&, ALGIF_BITMAP*);
        void read_palette(ALLEGRO_FILE* const& file, ALGIF_PALETTE* const& palette);
        void deinterlace(ALGIF_BITMAP* const& bmp);
        bool algif_load_raw(ALLEGRO_FILE*&, const bool = true);
        bool algif_load_animation_f(ALLEGRO_FILE*&);
        bool algif_load_animation(char const*);
        void algif_render_frame(int, int, int);
        void algif_destroy_animation();
        ALGIF_BITMAP* algif_create_bitmap(int, int);
        void algif_destroy_bitmap(ALGIF_BITMAP*&);
        void algif_blit(ALGIF_BITMAP* const&, ALGIF_BITMAP* const&, int, int, int, int, int, int);
        ALLEGRO_BITMAP* algif_get_bitmap(double) const;
        ALLEGRO_BITMAP* algif_get_frame_bitmap(int) const;
        double algif_get_frame_duration(int) const;

		ALLEGRO_BITMAP* get_for_draw() const override;
    public:
        GIF(const GIF&) = delete;
        void operator=(const GIF&) = delete;

        /// <summary>
        /// <para>Move a texture_gif to this.</para>
        /// </summary>
        /// <param name="{texture_functional}">A texture_functional.</param>
        GIF(GIF&&) noexcept;

        /// <summary>
        /// <para>Move a gif texture to this.</para>
        /// </summary>
        /// <param name="{texture_functional}">A texture_functional.</param>
        void operator=(GIF&&) noexcept;

        GIF() = default;

		/// <summary>
		/// <para>Load a gif from a file path.</para>
		/// </summary>
		/// <param name="{std::string}">Path.</param>
		/// <param name="{int}">Flags</param>
		/// <param name="{int}">Format</param>
		/// <returns>{bool} True if success.</returns>
        GIF(const std::string&, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);

		/// <summary>
		/// <para>Load a gif from file directly.</para>
		/// </summary>
		/// <param name="{ALLEGRO_FILE*}">A file.</param>
		/// <returns>{bool} True if success.</returns>
        GIF(ALLEGRO_FILE*);

		/// <summary>
		/// <para>Load a gif from a file path.</para>
		/// </summary>
		/// <param name="{std::string}">Path.</param>
		/// <param name="{int}">Flags</param>
		/// <param name="{int}">Format</param>
		/// <returns>{bool} True if success.</returns>
		bool load(const std::string&, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);

		/// <summary>
		/// <para>Load a gif from file directly.</para>
		/// </summary>
		/// <param name="{ALLEGRO_FILE*}">A file.</param>
		/// <returns>{bool} True if success.</returns>
		bool load(ALLEGRO_FILE*);

		/// <summary>
		/// <para>Get the width of the gif texture bitmap.</para>
		/// </summary>
		/// <returns>{int} Width.</returns>
		int get_width() const;

		/// <summary>
		/// <para>Get the height of the gif texture bitmap.</para>
		/// </summary>
		/// <returns>{int} Height.</returns>
		int get_height() const;

		/// <summary>
		/// <para>Get the raw bitmap pointer of this point in time.</para>
		/// </summary>
		/// <returns>{ALLEGRO_BITMAP*} Internal bitmap pointer.</returns>
		ALLEGRO_BITMAP* get_raw_bitmap() const;

		/// <summary>
		/// <para>Get the raw bitmap pointer of this point in time.</para>
		/// </summary>
		/// <returns>{ALLEGRO_BITMAP*} Internal bitmap pointer.</returns>
		operator ALLEGRO_BITMAP* () const;

		/// <summary>
		/// <para>It is considered valid if the bitmap and the gif buffer is not null.</para>
		/// </summary>
		/// <returns>{bool} Is there a gif?</returns>
		bool valid() const;

		/// <summary>
		/// <para>It's true if there's no bitmap and gif buffer loaded.</para>
		/// </summary>
		/// <returns>{bool} No gif around?</returns>
		bool empty() const;

		/// <summary>
		/// <para>Unload all frames and gif.</para>
		/// </summary>
		void destroy();

		/// <summary>
		/// <para>Get the average time of this GIF animation.</para>
		/// </summary>
		/// <returns>{double} Time, in seconds, or -1 if empty.</returns>
		double get_interval_average() const;

		/// <summary>
		/// <para>Get the longest interval between two frames of this GIF animation.</para>
		/// </summary>
		/// <returns>{double} Time, in seconds, or -1 if empty.</returns>
		double get_interval_longest() const;

		/// <summary>
		/// <para>Get the shortest interval between two frames of this GIF animation.</para>
		/// </summary>
		/// <returns>{double} Time, in seconds, or -1 if empty.</returns>
		double get_interval_shortest() const;

		/// <summary>
		/// <para>Get how many frames are in the animation.</para>
		/// <para>This is not called size() because it could mean texture size or something.</para>
		/// </summary>
		/// <returns>{size_t} </returns>
		size_t get_amount_of_frames() const;

		/// <summary>
		/// <para>Get a specific frame by index.</para>
		/// </summary>
		/// <param name="{size_t}">Frame number.</param>
		/// <returns>{ALLEGRO_BITMAP*} Raw bitmap (null if out of range or empty).</returns>
		ALLEGRO_BITMAP* index_raw(const size_t) const;

		/// <summary>
		/// <para>Get a specific frame as Bitmap by index.</para>
		/// </summary>
		/// <param name="{size_t}">Frame number.</param>
		/// <returns>{Bitmap} Bitmap (null if out of range or empty).</returns>
		Bitmap index(const size_t) const;

		/// <summary>
		/// <para>Get the screen time of this specific frame.</para>
		/// </summary>
		/// <returns>{double} Time, in seconds, or -1 if empty.</returns>
		double get_interval_of_index(const size_t) const;

		using Bitmap::get_flags;
		using Bitmap::get_format;
		using Bitmap::get_width;
		using Bitmap::get_height;
		using Bitmap::get_pixel;
		using Bitmap::draw;
	};
}