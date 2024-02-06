#include "../include/locked_region.h"

namespace AllegroCPP {

	Locked_region::Locked_region(ALLEGRO_BITMAP* bmp, const int format, const int flags)
		: m_locked(bmp), m_format(format), m_flags(flags)
	{
		// because BITMAP has to be loaded, assume system is installed.
		if (!bmp) throw std::invalid_argument("Bitmap was null!");

		if (!(m_ptr = al_lock_bitmap(bmp, format, flags))) throw std::runtime_error("Can't lock bitmap.");
	}

	Locked_region::Locked_region(ALLEGRO_BITMAP* bmp, const int pos_x, const int pos_y, const int width, const int height, const int format, const int flags)
		: m_locked(bmp), m_format(format), m_flags(flags)
	{
		// because BITMAP has to be loaded, assume system is installed.
		if (!bmp) throw std::invalid_argument("Bitmap was null!");

		if (!(m_ptr = al_lock_bitmap_region(bmp, pos_x, pos_y, width, height, format, flags))) throw std::runtime_error("Can't lock bitmap.");
	}

	Locked_region::~Locked_region()
	{
		if (m_ptr && m_locked) al_unlock_bitmap(m_locked);
	}

	Locked_region::Locked_region(Locked_region&& oth) noexcept
		: m_locked(oth.m_locked), m_ptr(oth.m_ptr), m_format(oth.m_format), m_flags(oth.m_flags)
	{
		oth.m_locked = nullptr;
		oth.m_ptr = nullptr;
	}

	void Locked_region::operator=(Locked_region&& oth) noexcept
	{
		if (m_ptr && m_locked) al_unlock_bitmap(m_locked);
		m_locked = oth.m_locked;
		m_ptr = oth.m_ptr;
		m_format = oth.m_format;
		m_flags = oth.m_flags;
		oth.m_locked = nullptr;
		oth.m_ptr = nullptr;
	}

	int Locked_region::get_format_pixel_size() const
	{
		return m_ptr ? al_get_pixel_size(m_format) : -1;
	}

	int Locked_region::get_format_bits() const
	{
		return m_ptr ? al_get_pixel_format_bits(m_format) : -1;
	}

	int Locked_region::get_pixel_block_size() const
	{
		return m_ptr ? al_get_pixel_block_size(m_format) : -1;
	}

	int Locked_region::get_pixel_block_width() const
	{
		return m_ptr ? al_get_pixel_block_width(m_format) : -1;
	}

	int Locked_region::get_pixel_block_height() const
	{
		return m_ptr ? al_get_pixel_block_height(m_format) : -1;
	}

	void* Locked_region::get_data()
	{
		return m_ptr ? m_ptr->data : nullptr;
	}

	const void* Locked_region::get_data() const
	{
		return m_ptr ? m_ptr->data : nullptr;
	}

	int Locked_region::get_format() const
	{
		return m_ptr ? m_ptr->format : -1;
	}

	int Locked_region::get_pitch() const
	{
		return m_ptr ? m_ptr->pitch : -1;
	}

	int Locked_region::get_pixel_size() const
	{
		return m_ptr ? m_ptr->pixel_size : -1;
	}

	bool Locked_region::put_pixel(const int pos_x, const int pos_y, const ALLEGRO_COLOR color)
	{
		if (!m_ptr || !m_locked) return false;
		ALLEGRO_BITMAP* targ = al_get_target_bitmap();
		al_set_target_bitmap(m_locked);
		al_put_pixel(pos_x, pos_y, color);
		al_set_target_bitmap(targ);
		return true;
	}

	bool Locked_region::put_blended_pixel(const int pos_x, const int pos_y, const ALLEGRO_COLOR color)
	{
		if (!m_ptr || !m_locked) return false;
		ALLEGRO_BITMAP* targ = al_get_target_bitmap();
		al_set_target_bitmap(m_locked);
		al_put_blended_pixel(pos_x, pos_y, color);
		al_set_target_bitmap(targ);
		return true;
	}

}