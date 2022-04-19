#pragma once

// future me: make lock_region eat an ALLEGRO_BITMAP and lock it while necessary.

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include <memory> // shared parent bitmap easy
#include <string>
#include <stdexcept>
#include <optional>
#include <variant>
#include <vector>

namespace AllegroCPP {

	const ALLEGRO_COLOR solid_white = al_map_rgb(255, 255, 255);

	struct bitmap_cut {
		int posx, posy, width, height;
	};
	struct bitmap_rotate_transform {
		float centerx, centery, rotationrad;
	};
	struct bitmap_scale {
		float scalex, scaley;
	};
	struct bitmap_parent_info {
		ALLEGRO_BITMAP* parent = nullptr;
		int offx, offy, width, height;
	};

	using bitmap_prop = std::variant<bitmap_cut, bitmap_rotate_transform, ALLEGRO_COLOR, bitmap_scale>;

	class Bitmap {
	public:
		enum class pixelrule{DEFAULT, AFFECTED_BY_TRANSFORM, AFFECTED_BY_BLENDER};
	private:
		std::shared_ptr<ALLEGRO_BITMAP> m_bmp, m_parent;
		std::shared_ptr<ALLEGRO_FILE> m_file; // reference to a file if bitmap loaded from it
	public:
		Bitmap(std::pair<int, int> size, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
		Bitmap(const Bitmap&, std::pair<int,int> subcut, std::pair<int,int> subsize, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
		Bitmap(const std::string& path, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
		Bitmap(std::shared_ptr<ALLEGRO_FILE> file, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0, const std::string& fileextensionincludingdot = {});
		~Bitmap();

		Bitmap(const Bitmap&); // clone
		Bitmap(Bitmap&&) noexcept; // move
		Bitmap& operator=(const Bitmap&); // clone
		void operator=(Bitmap&&) noexcept; // move

		bool empty() const;
		bool valid() const;
		operator bool() const;
		operator ALLEGRO_BITMAP* ();

		bool convert(int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);

		int get_flags() const;
		int get_format() const;
		int get_height() const;
		int get_width() const;

		ALLEGRO_COLOR get_pixel(std::pair<int, int> pos) const;
		bool is_locked() const;
		bool is_compatible() const;
		bool is_sub_bitmap() const;
		bitmap_parent_info get_parent_info();
		bool reparent(std::pair<int, int> subcut, std::pair<int, int> subsize);
		bool reparent(const Bitmap&, std::pair<int, int> subcut, std::pair<int, int> subsize);

		bool clear_to_color(ALLEGRO_COLOR color);
		bool draw(std::pair<float, float> target, std::vector<bitmap_prop> props = {}, int flags = 0);
		bool put_pixel(std::pair<int, int> pos, ALLEGRO_COLOR color, pixelrule rule = pixelrule::DEFAULT);

		bool set_as_target();

		bool set_clip_rectangle(std::pair<int, int> clipcut, std::pair<int, int> clipsize);
		bool get_clip_rectangle(int& posx, int& posy, int& width, int& height) const;
		bool reset_clip_rectangle();

		bool mask_to_alpha(ALLEGRO_COLOR);
	};
}