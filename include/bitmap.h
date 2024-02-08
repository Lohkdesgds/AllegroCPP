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
	struct bitmap_position_and_flags {
		float target_x, target_y;
		int flags = 0;
	};

	using bitmap_prop = std::variant<bitmap_cut, bitmap_rotate_transform, ALLEGRO_COLOR, bitmap_scale, bitmap_position_and_flags>;

	class Bitmap {
	public:
		enum class pixelrule{DEFAULT, AFFECTED_BY_TRANSFORM, AFFECTED_BY_BLENDER};
	private:
		std::shared_ptr<ALLEGRO_BITMAP> m_bmp, m_parent;
		bool m_treat_ref_const = false;
		std::shared_ptr<std::unique_ptr<ALLEGRO_FILE, std::function<void(ALLEGRO_FILE*)>>> m_file; // reference to a file if bitmap loaded from it
		
		Bitmap(ALLEGRO_BITMAP*, const bool treat_as_const);

		friend Bitmap make_const_bitmap_of(ALLEGRO_BITMAP*);

		struct draw_props {
			mutable bitmap_cut _cut = bitmap_cut{ 0, 0, 0, 0 };
			bitmap_rotate_transform _transf = bitmap_rotate_transform{ 0.0f, 0.0f, 0.0f };
			ALLEGRO_COLOR _color = al_map_rgb(255, 255, 255);
			bitmap_scale _scale = bitmap_scale{ 1.0f, 1.0f };
			bitmap_position_and_flags _pos_and_flag = {};
			
			void check(ALLEGRO_BITMAP*) const;
			void reset();
		};
		draw_props m_stored_draw_props;

		virtual ALLEGRO_BITMAP* get_for_draw() const;
	public:
		Bitmap() = default;
		Bitmap(const int size_x, const int size_y, const int flags = ALLEGRO_VIDEO_BITMAP, const int format = 0);
		Bitmap(const Bitmap&, const int subcut_x, const int subcut_y, const int subsize_x, const int subsize_y, const int flags = ALLEGRO_VIDEO_BITMAP, const int format = 0);
		Bitmap(const std::string& path, const int flags = ALLEGRO_VIDEO_BITMAP, const int format = 0);
		Bitmap(std::shared_ptr<std::unique_ptr<ALLEGRO_FILE, std::function<void(ALLEGRO_FILE*)>>> file, const int flags = ALLEGRO_VIDEO_BITMAP, const int format = 0, const std::string& fileextensionincludingdot = {});
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
		virtual bool convert(const int flags = ALLEGRO_VIDEO_BITMAP, const int format = 0);

		virtual int get_flags() const;
		virtual int get_format() const;
		virtual int get_height() const;
		virtual int get_width() const;

		virtual ALLEGRO_COLOR get_pixel(const int pos_x, const int pos_y) const;
		virtual bool is_locked() const;
		virtual bool is_compatible() const;
		virtual bool is_sub_bitmap() const;
		virtual bitmap_parent_info get_parent_info();
		virtual bool reparent(const int subcut_x, const int subcut_y, const int subsize_x, const int subsize_y);
		virtual bool reparent(const Bitmap&, const int subcut_x, const int subcut_y, const int subsize_x, const int subsize_y);

		virtual bool clear_to_color(const ALLEGRO_COLOR color);

		virtual void set_draw_property(bitmap_prop prop);
		virtual void set_draw_properties(std::vector<bitmap_prop> props);
		virtual void reset_draw_properties();

		virtual bool draw() const;
		virtual bool draw(const float target_x, const float target_y, const int flags = 0);
		
		virtual bool put_pixel(const int pos_x, const int pos_y, const ALLEGRO_COLOR color, const pixelrule rule = pixelrule::DEFAULT);

		virtual bool set_as_target();

		virtual bool set_clip_rectangle(const int clipcut_x, const int clipcut_y, const int clipsize_x, const int clipsize_y);
		virtual bool get_clip_rectangle(int& posx, int& posy, int& width, int& height) const;
		virtual bool reset_clip_rectangle();

		virtual bool mask_to_alpha(const ALLEGRO_COLOR);
	};

	Bitmap make_const_bitmap_of(ALLEGRO_BITMAP*);
}