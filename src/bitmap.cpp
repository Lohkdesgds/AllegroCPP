#include "../include/bitmap.h"

namespace AllegroCPP {

	Bitmap::Bitmap(ALLEGRO_BITMAP* cnst, const bool treat_as_const)
		: m_bmp(std::shared_ptr<ALLEGRO_BITMAP>(cnst, [](auto) {})), m_treat_ref_const(true)
	{
		if (!treat_as_const) throw std::runtime_error("Invalid setup of const ref of bitmap");
	}

	ALLEGRO_BITMAP* Bitmap::get_for_draw() const
	{
		return m_bmp.get();
	}

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

	Bitmap::Bitmap(int size_x, int size_y, int flags, int format)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_image_addon_initialized()) al_init_image_addon();

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_create_bitmap(size_x, size_y);
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

	Bitmap::Bitmap(const Bitmap& oth, int subcut_x, int subcut_y, int subsize_x, int subsize_y, int flags, int format)
		: m_parent(oth.m_bmp)
	{
		if (!m_parent) throw std::invalid_argument("Bitmap parent was null!");
		if (!al_is_system_installed()) al_init();
		if (!al_is_image_addon_initialized()) al_init_image_addon();

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_create_sub_bitmap(m_parent.get(), subcut_x, subcut_y, subsize_x, subsize_y);
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

		ALLEGRO_BITMAP* nbmp = al_clone_bitmap(get_for_draw());
		if (!nbmp) throw std::runtime_error("Cannot clone bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
		m_parent = oth.m_parent;
	}

	Bitmap::Bitmap(Bitmap&& oth) noexcept
		: m_bmp(std::move(oth.m_bmp)), m_parent(std::move(oth.m_parent)), m_file(std::move(oth.m_file)), m_treat_ref_const(oth.m_treat_ref_const)
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

		ALLEGRO_BITMAP* nbmp = al_clone_bitmap(get_for_draw());
		if (!nbmp) throw std::runtime_error("Cannot clone bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
		m_parent = oth.m_parent;

		return *this;
	}

	void Bitmap::operator=(Bitmap&& oth) noexcept
	{
		m_bmp = std::move(oth.m_bmp);
		m_parent = std::move(oth.m_parent);
		m_file = std::move(oth.m_file);
		m_treat_ref_const = oth.m_treat_ref_const;
	}

	bool Bitmap::empty() const
	{
		return get_for_draw() == nullptr;
	}

	bool Bitmap::valid() const
	{
		return get_for_draw() != nullptr;
	}

	Bitmap::operator bool() const
	{
		return get_for_draw() != nullptr;
	}

	Bitmap::operator ALLEGRO_BITMAP* ()
	{
		return get_for_draw();
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
		bmp.m_treat_ref_const = m_treat_ref_const;
		return bmp;
	}

	bool Bitmap::convert(int flags, int format)
	{
		if (!get_for_draw()) return false;
		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);
		al_convert_bitmap(get_for_draw());
		return true;
	}

	int Bitmap::get_flags() const
	{
		return get_for_draw() ? al_get_bitmap_flags(get_for_draw()) : -1;
	}

	int Bitmap::get_format() const
	{
		return get_for_draw() ? al_get_bitmap_format(get_for_draw()) : -1;
	}

	int Bitmap::get_height() const
	{
		return get_for_draw() ? al_get_bitmap_height(get_for_draw()) : -1;
	}

	int Bitmap::get_width() const
	{
		return get_for_draw() ? al_get_bitmap_width(get_for_draw()) : -1;
	}

	ALLEGRO_COLOR Bitmap::get_pixel(std::pair<int, int> pos) const
	{
		static const ALLEGRO_COLOR solid_white = al_map_rgb(255, 255, 255);
		return get_for_draw() ? al_get_pixel(get_for_draw(), pos.first, pos.second) : solid_white;
	}

	ALLEGRO_COLOR Bitmap::get_pixel(int pos_x, int pos_y) const
	{
		static const ALLEGRO_COLOR solid_white = al_map_rgb(255, 255, 255);
		return get_for_draw() ? al_get_pixel(get_for_draw(), pos_x, pos_y) : solid_white;
	}

	bool Bitmap::is_locked() const
	{
		return get_for_draw() ? al_is_bitmap_locked(get_for_draw()) : false;
	}

	bool Bitmap::is_compatible() const
	{
		return get_for_draw() ? al_is_compatible_bitmap(get_for_draw()) : false;
	}

	bool Bitmap::is_sub_bitmap() const
	{
		return get_for_draw() ? al_is_sub_bitmap(get_for_draw()) : false;
	}

	bitmap_parent_info Bitmap::get_parent_info()
	{
		bitmap_parent_info info{};
		if (!get_for_draw()) return info;
		info.offx = al_get_bitmap_x(get_for_draw());
		info.offy = al_get_bitmap_y(get_for_draw());
		info.width = get_width();
		info.height = get_height();
		info.parent = m_parent.get();
		return info;
	}

	bool Bitmap::reparent(std::pair<int, int> subcut, std::pair<int, int> subsize)
	{
		if (!m_bmp || !m_parent) return false;
		al_reparent_bitmap(get_for_draw(), m_parent.get(), subcut.first, subcut.second, subsize.first, subsize.second);
		return true;
	}

	bool Bitmap::reparent(int subcut_x, int subcut_y, int subsize_x, int subsize_y)
	{
		if (!m_bmp || !m_parent) return false;
		al_reparent_bitmap(get_for_draw(), m_parent.get(), subcut_x, subcut_y, subsize_x, subsize_y);
		return true;
	}

	bool Bitmap::reparent(const Bitmap& oth, std::pair<int, int> subcut, std::pair<int, int> subsize)
	{
		if (!m_bmp || !oth.m_bmp) return false;
		m_parent = oth.m_bmp;
		m_file = oth.m_file;
		al_reparent_bitmap(get_for_draw(), m_parent.get(), subcut.first, subcut.second, subsize.first, subsize.second);
		return true;
	}

	bool Bitmap::reparent(const Bitmap& oth, int subcut_x, int subcut_y, int subsize_x, int subsize_y)
	{
		if (!m_bmp || !oth.m_bmp) return false;
		m_parent = oth.m_bmp;
		m_file = oth.m_file;
		al_reparent_bitmap(get_for_draw(), m_parent.get(), subcut_x, subcut_y, subsize_x, subsize_y);
		return true;
	}

	bool Bitmap::clear_to_color(ALLEGRO_COLOR color)
	{
		if (!get_for_draw() || !m_parent || m_treat_ref_const) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(get_for_draw());
		al_clear_to_color(color);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::draw(std::pair<float, float> target, std::vector<bitmap_prop> props, int flags)
	{
		static const ALLEGRO_COLOR solid_white = al_map_rgb(255, 255, 255);
		if (!get_for_draw()) return false;

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

		al_draw_tinted_scaled_rotated_bitmap_region(get_for_draw(),
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

	bool Bitmap::draw(float target_x, float target_y, std::vector<bitmap_prop> props, int flags)
	{
		static const ALLEGRO_COLOR solid_white = al_map_rgb(255, 255, 255);
		if (!get_for_draw()) return false;

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

		al_draw_tinted_scaled_rotated_bitmap_region(get_for_draw(),
			_cut.posx, _cut.posy, _cut.width, _cut.height,
			_color,
			_transf.centerx, _transf.centery,
			target_x, target_y,
			_scale.scalex, _scale.scaley,
			_transf.rotationrad,
			flags
		);

		return true;
	}

	bool Bitmap::put_pixel(std::pair<int, int> pos, ALLEGRO_COLOR color, pixelrule rule)
	{
		if (!get_for_draw() || m_treat_ref_const) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(get_for_draw());
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

	bool Bitmap::put_pixel(int pos_x, int pos_y, ALLEGRO_COLOR color, pixelrule rule)
	{
		if (!get_for_draw() || m_treat_ref_const) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(get_for_draw());
		switch (rule) {
		case pixelrule::DEFAULT:
			al_put_pixel(pos_x, pos_y, color);
			break;
		case pixelrule::AFFECTED_BY_TRANSFORM:
			al_draw_pixel(pos_x, pos_y, color);
			break;
		case pixelrule::AFFECTED_BY_BLENDER:
			al_put_blended_pixel(pos_x, pos_y, color);
			break;
		}
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::set_as_target()
	{
		if (!get_for_draw() || m_treat_ref_const) return false;
		al_set_target_bitmap(get_for_draw());
		return true;
	}

	bool Bitmap::set_clip_rectangle(std::pair<int, int> clipcut, std::pair<int, int> clipsize)
	{
		if (!get_for_draw() || m_treat_ref_const) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(get_for_draw());
		al_set_clipping_rectangle(clipcut.first, clipcut.second, clipsize.first, clipsize.second);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::set_clip_rectangle(int clipcut_x, int clipcut_y, int clipsize_x, int clipsize_y)
	{
		if (!get_for_draw() || m_treat_ref_const) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(get_for_draw());
		al_set_clipping_rectangle(clipcut_x, clipcut_y, clipsize_x, clipsize_y);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::get_clip_rectangle(int& posx, int& posy, int& width, int& height) const
	{
		if (!get_for_draw() || m_treat_ref_const) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(get_for_draw());
		al_get_clipping_rectangle(&posx, &posy, &width, &height);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::reset_clip_rectangle()
	{
		if (!get_for_draw() || m_treat_ref_const) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(get_for_draw());
		al_reset_clipping_rectangle();
		al_set_target_bitmap(oldtarg);
		return true;
	}

	bool Bitmap::mask_to_alpha(ALLEGRO_COLOR color)
	{
		if (!get_for_draw() || m_treat_ref_const) return false;
		al_convert_mask_to_alpha(get_for_draw(), color);
		return true;
	}

	Bitmap make_const_bitmap_of(ALLEGRO_BITMAP* bmp)
	{
		Bitmap nbmp(bmp, true); // manual const ref
		return nbmp;
	}
}