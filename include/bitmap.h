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
#include <functional>

namespace AllegroCPP {

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
		bool m_treat_ref_const = false;
		std::shared_ptr<std::unique_ptr<ALLEGRO_FILE, std::function<void(ALLEGRO_FILE*)>>> m_file; // reference to a file if bitmap loaded from it
		
		Bitmap(ALLEGRO_BITMAP*, const bool treat_as_const);

		friend Bitmap make_const_bitmap_of(ALLEGRO_BITMAP*);

		virtual ALLEGRO_BITMAP* get_for_draw() const;
	public:
		Bitmap() = default;
		Bitmap(std::pair<int, int> size, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
		Bitmap(int size_x, int size_y, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
		Bitmap(const Bitmap&, std::pair<int,int> subcut, std::pair<int,int> subsize, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
		Bitmap(const Bitmap&, int subcut_x, int subcut_y, int subsize_x, int subsize_y, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
		Bitmap(const std::string& path, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);
		Bitmap(std::shared_ptr<std::unique_ptr<ALLEGRO_FILE, std::function<void(ALLEGRO_FILE*)>>> file, int flags = ALLEGRO_VIDEO_BITMAP, int format = 0, const std::string& fileextensionincludingdot = {});
		~Bitmap();

		Bitmap(const Bitmap&); // clone
		Bitmap(Bitmap&&) noexcept; // move
		Bitmap& operator=(const Bitmap&); // clone
		void operator=(Bitmap&&) noexcept; // move

		virtual bool empty() const;
		virtual bool valid() const;
		virtual operator bool() const;
		virtual operator ALLEGRO_BITMAP* ();

		virtual void destroy();

		virtual Bitmap make_ref() const;
		virtual bool convert(int flags = ALLEGRO_VIDEO_BITMAP, int format = 0);

		virtual int get_flags() const;
		virtual int get_format() const;
		virtual int get_height() const;
		virtual int get_width() const;

		virtual ALLEGRO_COLOR get_pixel(std::pair<int, int> pos) const;
		virtual ALLEGRO_COLOR get_pixel(int pos_x, int pos_y) const;
		virtual bool is_locked() const;
		virtual bool is_compatible() const;
		virtual bool is_sub_bitmap() const;
		virtual bitmap_parent_info get_parent_info();
		virtual bool reparent(std::pair<int, int> subcut, std::pair<int, int> subsize);
		virtual bool reparent(int subcut_x, int subcut_y, int subsize_x, int subsize_y);
		virtual bool reparent(const Bitmap&, std::pair<int, int> subcut, std::pair<int, int> subsize);
		virtual bool reparent(const Bitmap&, int subcut_x, int subcut_y, int subsize_x, int subsize_y);

		virtual bool clear_to_color(ALLEGRO_COLOR color);
		virtual bool draw(std::pair<float, float> target, std::vector<bitmap_prop> props = {}, int flags = 0);
		virtual bool draw(float target_x, float target_y, std::vector<bitmap_prop> props = {}, int flags = 0);
		virtual bool put_pixel(std::pair<int, int> pos, ALLEGRO_COLOR color, pixelrule rule = pixelrule::DEFAULT);
		virtual bool put_pixel(int pos_x, int pos_y, ALLEGRO_COLOR color, pixelrule rule = pixelrule::DEFAULT);

		virtual bool set_as_target();

		virtual bool set_clip_rectangle(std::pair<int, int> clipcut, std::pair<int, int> clipsize);
		virtual bool set_clip_rectangle(int clipcut_x, int clipcut_y, int clipsize_x, int clipsize_y);
		virtual bool get_clip_rectangle(int& posx, int& posy, int& width, int& height) const;
		virtual bool reset_clip_rectangle();

		virtual bool mask_to_alpha(ALLEGRO_COLOR);
	};

	Bitmap make_const_bitmap_of(ALLEGRO_BITMAP*);
}