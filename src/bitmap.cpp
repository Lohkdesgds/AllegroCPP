#include "../include/bitmap.h"

namespace AllegroCPP {

	Bitmap::Bitmap(std::pair<int, int> size, int flags, int format)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_image_addon_initialized()) al_init_image_addon();

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_create_bitmap(size.first, size.second);
		if (!nbmp) throw std::runtime_error("Cannot create bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	}

	Bitmap::Bitmap(const Bitmap& oth, std::pair<int, int> subcut, std::pair<int, int> subsize, int flags, int format)
		: m_parent(oth.m_bmp)
	{
		if (!m_parent) throw std::invalid_argument("Bitmap parent was null!");
		if (!al_is_system_installed()) al_init();
		if (!al_is_image_addon_initialized()) al_init_image_addon();

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_create_sub_bitmap(m_parent.get(), subcut.first, subcut.second, subsize.first, subsize.second);
		if (!nbmp) throw std::runtime_error("Cannot create sub bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	}

	Bitmap::Bitmap(const std::string& path, int flags, int format)
	{
		if (path.empty()) throw std::invalid_argument("Path was empty!");
		if (!al_is_system_installed()) al_init();
		if (!al_is_image_addon_initialized()) al_init_image_addon();

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_load_bitmap(path.c_str());
		if (!nbmp) throw std::runtime_error("Cannot load bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	}

	Bitmap::Bitmap(std::shared_ptr<std::unique_ptr<ALLEGRO_FILE, std::function<void(ALLEGRO_FILE*)>>> file, int flags, int format, const std::string& fileextensionincludingdot)
		: m_file(file)
	{
		if (!m_file) throw std::invalid_argument("File was null");
		if (!al_is_system_installed()) al_init();
		if (!al_is_image_addon_initialized()) al_init_image_addon();

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_load_bitmap_f(m_file->get(), fileextensionincludingdot.empty() ? nullptr : fileextensionincludingdot.c_str());
		if (!nbmp) throw std::runtime_error("Cannot load bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	}

	Bitmap::~Bitmap()
	{
		destroy();
	}

	Bitmap::Bitmap(const Bitmap& oth)
	{
		if (!oth.m_bmp) throw std::runtime_error("Invalid bitmap to clone!");
		if (!al_is_system_installed()) al_init();
		if (!al_is_image_addon_initialized()) al_init_image_addon();

		int flags = oth.get_flags();
		int format = oth.get_format();
		if (flags < 0) flags = 0;
		if (format < 0) format = 0;

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_clone_bitmap(m_parent.get());
		if (!nbmp) throw std::runtime_error("Cannot clone bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	}

	Bitmap::Bitmap(Bitmap&& oth) noexcept
		: m_bmp(std::move(oth.m_bmp)), m_parent(std::move(oth.m_parent)), m_file(std::move(oth.m_file))
	{
	}

	Bitmap& Bitmap::operator=(const Bitmap& oth)
	{
		if (!oth.m_bmp) throw std::runtime_error("Invalid bitmap to clone!");

		int flags = oth.get_flags();
		int format = oth.get_format();
		if (flags < 0) flags = 0;
		if (format < 0) format = 0;

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_clone_bitmap(m_parent.get());
		if (!nbmp) throw std::runtime_error("Cannot clone bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });

		return *this;
	}

	void Bitmap::operator=(Bitmap&& oth) noexcept
	{
		m_bmp = std::move(oth.m_bmp);
		m_parent = std::move(oth.m_parent);
		m_file = std::move(oth.m_file);
	}

	bool Bitmap::empty() const
	{
		return m_bmp.get() == nullptr;
	}

	bool Bitmap::valid() const
	{
		return m_bmp.get() != nullptr;
	}

	Bitmap::operator bool() const
	{
		return m_bmp.get() != nullptr;
	}

	Bitmap::operator ALLEGRO_BITMAP* ()
	{
		return m_bmp.get();
	}

	void Bitmap::destroy()
	{
		m_bmp.reset();
		m_parent.reset();
		m_file.reset();
	}

	Bitmap Bitmap::make_ref() const
	{
		Bitmap bmp;
		bmp.m_bmp = m_bmp;
		bmp.m_parent = m_parent;
		bmp.m_file = m_file;
		return bmp;
	}

	bool Bitmap::convert(int flags, int format)
	{
		if (!m_bmp) return false;
		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);
		al_convert_bitmap(m_bmp.get());
		return true;
	}

	int Bitmap::get_flags() const
	{
		return m_bmp ? al_get_bitmap_flags(m_bmp.get()) : -1;
	}

	int Bitmap::get_format() const
	{
		return m_bmp ? al_get_bitmap_format(m_bmp.get()) : -1;
	}

	int Bitmap::get_height() const
	{
		return m_bmp ? al_get_bitmap_height(m_bmp.get()) : -1;
	}

	int Bitmap::get_width() const
	{
		return m_bmp ? al_get_bitmap_width(m_bmp.get()) : -1;
	}

	ALLEGRO_COLOR Bitmap::get_pixel(std::pair<int, int> pos) const
	{
		static const ALLEGRO_COLOR solid_white = al_map_rgb(255, 255, 255);
		return m_bmp ? al_get_pixel(m_bmp.get(), pos.first, pos.second) : solid_white;
	}

	bool Bitmap::is_locked() const
	{
		return m_bmp ? al_is_bitmap_locked(m_bmp.get()) : false;
	}

	bool Bitmap::is_compatible() const
	{
		return m_bmp ? al_is_compatible_bitmap(m_bmp.get()) : false;
	}

	bool Bitmap::is_sub_bitmap() const
	{
		return m_bmp ? al_is_sub_bitmap(m_bmp.get()) : false;
	}

	bitmap_parent_info Bitmap::get_parent_info()
	{
		bitmap_parent_info info{};
		if (!m_bmp) return info;
		info.offx = al_get_bitmap_x(m_bmp.get());
		info.offy = al_get_bitmap_y(m_bmp.get());
		info.width = get_width();
		info.height = get_height();
		info.parent = m_parent.get();
		return info;
	}

	bool Bitmap::reparent(std::pair<int, int> subcut, std::pair<int, int> subsize)
	{
		if (!m_bmp || !m_parent) return false;
		al_reparent_bitmap(m_bmp.get(), m_parent.get(), subcut.first, subcut.second, subsize.first, subsize.second);
		return true;
	}

	bool Bitmap::reparent(const Bitmap& oth, std::pair<int, int> subcut, std::pair<int, int> subsize)
	{
		if (!m_bmp || !oth.m_bmp) return false;
		m_parent = oth.m_bmp;
		m_file = oth.m_file;
		al_reparent_bitmap(m_bmp.get(), m_parent.get(), subcut.first, subcut.second, subsize.first, subsize.second);
		return true;
	}

	bool Bitmap::clear_to_color(ALLEGRO_COLOR color)
	{
		if (!m_bmp || !m_parent) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(m_bmp.get());
		al_clear_to_color(color);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::draw(std::pair<float, float> target, std::vector<bitmap_prop> props, int flags)
	{
		static const ALLEGRO_COLOR solid_white = al_map_rgb(255, 255, 255);
		if (!m_bmp) return false;

		bitmap_cut _cut = bitmap_cut{ 0, 0, get_width(), get_height() };
		bitmap_rotate_transform _transf = bitmap_rotate_transform{ 0.0f, 0.0f, 0.0f };
		ALLEGRO_COLOR _color = solid_white;
		bitmap_scale _scale = bitmap_scale{ 1.0f, 1.0f };

		for (const auto& i : props) {
			if (std::holds_alternative<bitmap_cut>(i)) _cut = std::get<bitmap_cut>(i);
			else if (std::holds_alternative<bitmap_rotate_transform>(i)) _transf = std::get<bitmap_rotate_transform>(i);
			else if (std::holds_alternative<ALLEGRO_COLOR>(i)) _color = std::get<ALLEGRO_COLOR>(i);
			else if (std::holds_alternative<bitmap_scale>(i)) _scale = std::get<bitmap_scale>(i);
		}

		al_draw_tinted_scaled_rotated_bitmap_region(m_bmp.get(),
			_cut.posx, _cut.posy, _cut.width, _cut.height,
			_color,
			_transf.centerx, _transf.centery,
			target.first, target.second,
			_scale.scalex, _scale.scaley,
			_transf.rotationrad,
			flags
		);

		return true;
	}

	bool Bitmap::put_pixel(std::pair<int, int> pos, ALLEGRO_COLOR color, pixelrule rule)
	{
		if (!m_bmp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(m_bmp.get());
		switch (rule) {
		case pixelrule::DEFAULT:
			al_put_pixel(pos.first, pos.second, color);
			break;
		case pixelrule::AFFECTED_BY_TRANSFORM:
			al_draw_pixel(pos.first, pos.second, color);
			break;
		case pixelrule::AFFECTED_BY_BLENDER:
			al_put_blended_pixel(pos.first, pos.second, color);
			break;
		}
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::set_as_target()
	{
		if (!m_bmp) return false;
		al_set_target_bitmap(m_bmp.get());
		return true;
	}

	bool Bitmap::set_clip_rectangle(std::pair<int, int> clipcut, std::pair<int, int> clipsize)
	{
		if (!m_bmp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(m_bmp.get());
		al_set_clipping_rectangle(clipcut.first, clipcut.second, clipsize.first, clipsize.second);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::get_clip_rectangle(int& posx, int& posy, int& width, int& height) const
	{
		if (!m_bmp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(m_bmp.get());
		al_get_clipping_rectangle(&posx, &posy, &width, &height);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::reset_clip_rectangle()
	{
		if (!m_bmp) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(m_bmp.get());
		al_reset_clipping_rectangle();
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::mask_to_alpha(ALLEGRO_COLOR color)
	{
		if (!m_bmp) return false;
		al_convert_mask_to_alpha(m_bmp.get(), color);
		return true;
	}

}