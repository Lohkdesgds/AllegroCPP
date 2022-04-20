#pragma once

#include <allegro5/allegro.h>

#include <utility>
#include <stdexcept>

namespace AllegroCPP {

	class Locked_region {
		ALLEGRO_BITMAP* m_locked = nullptr;
		ALLEGRO_LOCKED_REGION* m_ptr = nullptr;
		int m_format, m_flags;
	public:
		Locked_region(ALLEGRO_BITMAP* bmp, int format = ALLEGRO_LOCK_READWRITE, int flags = ALLEGRO_PIXEL_FORMAT_ANY);
		Locked_region(ALLEGRO_BITMAP* bmp, std::pair<int,int> pos, std::pair<int,int> size, int format = ALLEGRO_LOCK_READWRITE, int flags = ALLEGRO_PIXEL_FORMAT_ANY);
		~Locked_region();

		Locked_region(const Locked_region&) = delete;
		Locked_region(Locked_region&&) noexcept;
		void operator=(const Locked_region&) = delete;
		void operator=(Locked_region&&) noexcept;

		int get_format_pixel_size() const;
		int get_format_bits() const;
		int get_pixel_block_size() const;
		int get_pixel_block_width() const;
		int get_pixel_block_height() const;

		void* get_data();
		const void* get_data() const;
		int get_format() const;
		int get_pitch() const;
		int get_pixel_size() const;

		bool put_pixel(std::pair<int, int> pos, ALLEGRO_COLOR color);
		bool put_blended_pixel(std::pair<int, int> pos, ALLEGRO_COLOR color);
	};

}