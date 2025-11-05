#include "video.h"

#include <utility>

namespace AllegroCPP {

	Video_event::Video_event(const ALLEGRO_EVENT ev)
		: m_ev(ev)
	{
	}

	ALLEGRO_VIDEO* Video_event::get_source() const
	{
		return (ALLEGRO_VIDEO*)m_ev.user.data1;
	}

	Video Video_event::get_video_source() const
	{
		return Video(get_source());
	}

	ALLEGRO_BITMAP* Video::get_for_draw() const
	{
		return m_video ? al_get_video_frame(m_video) : nullptr;
	}

	Video::Video(Video&& oth) noexcept
		: m_video(std::exchange(oth.m_video, nullptr))
	{
	}

	void Video::operator=(Video&& oth) noexcept
	{
		destroy();
		m_video = std::exchange(oth.m_video, nullptr);
	}

	Video::Video()
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_video_addon_initialized()) al_init_video_addon();
	}

	Video::Video(const std::string& str)
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_video_addon_initialized()) al_init_video_addon();

		m_video = al_open_video(str.c_str());
	}

	Video::Video(ALLEGRO_VIDEO* v)
		: m_video(v), m_delete_auto(false)
	{
	}

	Video::~Video()
	{
		destroy();
	}

	bool Video::start(ALLEGRO_MIXER* m)
	{
		if (m_video) {
			al_start_video(m_video, m);
			return true;
		}
		return false;
	}

	bool Video::start(ALLEGRO_VOICE* v)
	{
		if (m_video) {
			al_start_video_with_voice(m_video, v);
			return true;
		}
		return false;
	}

	void Video::destroy()
	{
		if (m_video) {
			if (m_delete_auto) al_close_video(m_video);

			m_video = nullptr;
		}
	}

	void Video::set_playing(const bool b)
	{
		if (m_video) al_set_video_playing(m_video, b);
	}

	bool Video::is_playing() const
	{
		return m_video ? al_is_video_playing(m_video) : false;
	}

	double Video::get_audio_rate() const
	{
		return m_video ? al_get_video_audio_rate(m_video) : 0.0;
	}

	double Video::get_fps() const
	{
		return m_video ? al_get_video_fps(m_video) : 0.0;
	}

	float Video::get_scaled_width() const
	{
		return m_video ? al_get_video_scaled_width(m_video) : 0.0f;
	}

	float Video::get_scaled_height() const
	{
		return m_video ? al_get_video_scaled_height(m_video) : 0.0f;
	}

	ALLEGRO_BITMAP* Video::get_frame() const
	{
		return get_for_draw();
	}

	Video::operator ALLEGRO_BITMAP* () const
	{
		return get_for_draw();
	}

	double Video::get_position(const ALLEGRO_VIDEO_POSITION_TYPE type) const
	{
		return m_video ? al_get_video_position(m_video, type) : 0.0;
	}

	bool Video::seek(const double pos, const Video::dir dr)
	{
		return m_video ? al_seek_video(m_video, dr == Video::dir::SET ? pos : (get_position() + pos)) : false;
	}

	ALLEGRO_EVENT_SOURCE* Video::get_event_source() const
	{
		return m_video ? al_get_video_event_source(m_video) : nullptr;
	}

	Video::operator ALLEGRO_EVENT_SOURCE* () const
	{
		return m_video ? al_get_video_event_source(m_video) : nullptr;
	}

	bool Video::operator==(const ALLEGRO_VIDEO* v) const
	{
		return m_video == v;
	}

	bool Video::operator!=(const ALLEGRO_VIDEO* v) const
	{
		return m_video != v;
	}

	std::string Video::identify(const std::string& str)
	{
		return al_identify_video(str.c_str());
	}

	std::string Video::identify_f(ALLEGRO_FILE* f)
	{
		return al_identify_video_f(f);
	}

}