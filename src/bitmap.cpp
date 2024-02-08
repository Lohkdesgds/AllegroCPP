#include "../include/bitmap.h"

namespace AllegroCPP {

	Bitmap::Bitmap(ALLEGRO_BITMAP* cnst, const bool treat_as_const)
		: m_bmp(std::shared_ptr<ALLEGRO_BITMAP>(cnst, [](auto) {})), m_treat_ref_const(true)
	{
		if (!treat_as_const) throw std::runtime_error("Invalid setup of const ref of bitmap");
	}

	void Bitmap::draw_props::check(ALLEGRO_BITMAP* b) const
	{
		if (_cut.width == 0 || _cut.height == 0) {
			_cut.width = al_get_bitmap_width(b);
			_cut.height = al_get_bitmap_height(b);
		}
	}

	void Bitmap::draw_props::reset()
	{
		*this = {}; // reset to defaults
	}

	ALLEGRO_BITMAP* Bitmap::get_for_draw() const
	{
		return m_bmp.get();
	}

	Bitmap::Bitmap(const int size_x, const int size_y, const int flags, const int format)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_image_addon_initialized()) al_init_image_addon();

		al_set_new_bitmap_flags(flags);
		al_set_new_bitmap_format(format);

		ALLEGRO_BITMAP* nbmp = al_create_bitmap(size_x, size_y);
		if (!nbmp) throw std::runtime_error("Cannot create bitmap!");

		m_bmp = std::shared_ptr<ALLEGRO_BITMAP>(nbmp, [](ALLEGRO_BITMAP* b) { al_destroy_bitmap(b); });
	}

	Bitmap::Bitmap(const Bitmap& oth, const int subcut_x, const int subcut_y, const int subsize_x, const int subsize_y, const int flags, const int format)
		: m_parent(oth.m_bmp), m_stored_draw_props(oth.m_stored_draw_props)
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

	Bitmap::Bitmap(const std::string& path, const int flags, const int format)
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

	Bitmap::Bitmap(std::shared_ptr<std::unique_ptr<ALLEGRO_FILE, std::function<void(ALLEGRO_FILE*)>>> file, const int flags, const int format, const std::string& fileextensionincludingdot)
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

		m_stored_draw_props = oth.m_stored_draw_props;

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
		: m_bmp(std::move(oth.m_bmp)), m_parent(std::move(oth.m_parent)), m_file(std::move(oth.m_file)), m_treat_ref_const(oth.m_treat_ref_const), m_stored_draw_props(oth.m_stored_draw_props)
	{
		oth.m_stored_draw_props.reset();
	}

	Bitmap& Bitmap::operator=(const Bitmap& oth)
	{
		if (!oth.m_bmp) throw std::runtime_error("Invalid bitmap to clone!");

		m_stored_draw_props = oth.m_stored_draw_props;

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
		m_stored_draw_props = oth.m_stored_draw_props;
		oth.m_stored_draw_props.reset();
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
		bmp.m_stored_draw_props = m_stored_draw_props;
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

	ALLEGRO_COLOR Bitmap::get_pixel(const int pos_x, const int pos_y) const
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

	bool Bitmap::reparent(const int subcut_x, const int subcut_y, const int subsize_x, const int subsize_y)
	{
		if (!m_bmp || !m_parent) return false;
		al_reparent_bitmap(get_for_draw(), m_parent.get(), subcut_x, subcut_y, subsize_x, subsize_y);
		return true;
	}

	bool Bitmap::reparent(const Bitmap& oth, const int subcut_x, const int subcut_y, const int subsize_x, const int subsize_y)
	{
		if (!m_bmp || !oth.m_bmp) return false;
		m_parent = oth.m_bmp;
		m_file = oth.m_file;
		al_reparent_bitmap(get_for_draw(), m_parent.get(), subcut_x, subcut_y, subsize_x, subsize_y);
		return true;
	}

	bool Bitmap::clear_to_color(const ALLEGRO_COLOR color)
	{
		if (!get_for_draw() || !m_parent || m_treat_ref_const) return false;
		ALLEGRO_BITMAP* oldtarg = al_get_target_bitmap();
		al_set_target_bitmap(get_for_draw());
		al_clear_to_color(color);
		al_set_target_bitmap(oldtarg);
		return true;
	}

	void Bitmap::set_draw_property(bitmap_prop prop)
	{
		if (std::holds_alternative<bitmap_cut>(prop)) m_stored_draw_props._cut = std::get<bitmap_cut>(prop);
		else if (std::holds_alternative<bitmap_rotate_transform>(prop)) m_stored_draw_props._transf = std::get<bitmap_rotate_transform>(prop);
		else if (std::holds_alternative<ALLEGRO_COLOR>(prop)) m_stored_draw_props._color = std::get<ALLEGRO_COLOR>(prop);
		else if (std::holds_alternative<bitmap_scale>(prop)) m_stored_draw_props._scale = std::get<bitmap_scale>(prop);
		else if (std::holds_alternative<bitmap_position_and_flags>(prop)) m_stored_draw_props._pos_and_flag = std::get<bitmap_position_and_flags>(prop);
	}

	void Bitmap::set_draw_properties(std::vector<bitmap_prop> props)
	{
		for (const auto& i : props) set_draw_property(i);
	}

	void Bitmap::reset_draw_properties()
	{
		m_stored_draw_props.reset();
	}

	bool Bitmap::draw() const
	{
		auto* to_draw = get_for_draw();
		if (!to_draw) return false;

		m_stored_draw_props.check(to_draw);

		al_draw_tinted_scaled_rotated_bitmap_region(to_draw,
			m_stored_draw_props._cut.posx, m_stored_draw_props._cut.posy, m_stored_draw_props._cut.width, m_stored_draw_props._cut.height,
			m_stored_draw_props._color,
			m_stored_draw_props._transf.centerx, m_stored_draw_props._transf.centery,
			m_stored_draw_props._pos_and_flag.target_x, m_stored_draw_props._pos_and_flag.target_y,
			m_stored_draw_props._scale.scalex, m_stored_draw_props._scale.scaley,
			m_stored_draw_props._transf.rotationrad,
			m_stored_draw_props._pos_and_flag.flags
		);

		return true;
	}
	
	bool Bitmap::draw(const float target_x, const float target_y, const int flags)
	{
		m_stored_draw_props._pos_and_flag.target_x = target_x;
		m_stored_draw_props._pos_and_flag.target_y = target_y;
		m_stored_draw_props._pos_and_flag.flags = flags;

		return draw();
	}

	bool Bitmap::put_pixel(const int pos_x, const int pos_y, const ALLEGRO_COLOR color, const pixelrule rule)
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

	bool Bitmap::set_clip_rectangle(const int clipcut_x, const int clipcut_y, const int clipsize_x, const int clipsize_y)
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

	bool Bitmap::mask_to_alpha(const ALLEGRO_COLOR color)
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