#include "../include/audio_stream.h"

namespace AllegroCPP {

	extern void __audio_allegro_start(int);

	Audio_stream::Audio_stream(Audio_stream&& oth)
		: m_stream(std::exchange(oth.m_stream, nullptr)), m_fileload(std::move(oth.m_fileload))
	{
	}

	void Audio_stream::operator=(Audio_stream&& oth)
	{
		m_stream = std::exchange(oth.m_stream, nullptr);
		m_fileload = std::move(oth.m_fileload);
	}

	Audio_stream::Audio_stream(const std::string& path, const size_t sample_count, const unsigned samples)
	{
		__audio_allegro_start(0);
		if (!load(path, sample_count, samples)) throw std::invalid_argument("Cannot load audio stream from file.");
	}

	Audio_stream::Audio_stream(File_shareable_ptr fp, const size_t sample_count, const unsigned samples)
	{
		__audio_allegro_start(0);
		if (!load(fp, sample_count, samples)) throw std::invalid_argument("Cannot load audio stream from file.");
	}

	Audio_stream::Audio_stream(File_shareable_ptr fp, const std::string& ext, const size_t sample_count, const unsigned samples)
	{
		__audio_allegro_start(0);
		if (!load(fp, ext, sample_count, samples)) throw std::invalid_argument("Cannot load audio stream from file.");
	}

	Audio_stream::Audio_stream(const size_t frag_count, const unsigned frag_samples, const unsigned freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF conf)
	{
		__audio_allegro_start(0);
		if (!create(frag_count, frag_samples, freq, depth, conf)) throw std::invalid_argument("Cannot create audio stream.");
	}

	bool Audio_stream::load(const std::string& path, const size_t sample_count, const unsigned samples)
	{
		destroy();
		return (m_stream = al_load_audio_stream(path.c_str(), sample_count, samples));
	}

	bool Audio_stream::load(File_shareable_ptr fp, const size_t sample_count, const unsigned samples)
	{
		destroy();
		m_fileload = fp;
		return (m_stream = al_load_audio_stream_f(fp->get(), al_identify_sample_f(fp->get()), sample_count, samples));
	}

	bool Audio_stream::load(File_shareable_ptr fp, const std::string& ext, const size_t sample_count, const unsigned samples)
	{
		destroy();
		m_fileload = fp;
		return (m_stream = al_load_audio_stream_f(fp->get(), ext.c_str(), sample_count, samples));
	}

	bool Audio_stream::create(const size_t frag_count, const unsigned frag_samples, const unsigned freq, ALLEGRO_AUDIO_DEPTH depth, ALLEGRO_CHANNEL_CONF conf)
	{
		destroy();
		return (m_stream = al_create_audio_stream(frag_count, frag_samples, freq, depth, conf));
	}

	ALLEGRO_EVENT_SOURCE* Audio_stream::get_event_source() const
	{
		return m_stream ? al_get_audio_stream_event_source(m_stream) : nullptr;
	}

	Audio_stream::operator ALLEGRO_EVENT_SOURCE* () const
	{
		return m_stream ? al_get_audio_stream_event_source(m_stream) : nullptr;
	}

	void Audio_stream::drain() const
	{
		if (m_stream) al_drain_audio_stream(m_stream);
	}

	bool Audio_stream::rewind()
	{
		if (m_stream) return al_rewind_audio_stream(m_stream);
		return false;
	}

	void Audio_stream::destroy()
	{
		if (m_stream) {
			al_destroy_audio_stream(std::exchange(m_stream, nullptr));
			m_fileload.reset();
		}
	}

	bool Audio_stream::detach() const
	{
		if (m_stream) return al_detach_audio_stream(m_stream);
		return false;
	}

	ALLEGRO_CHANNEL_CONF Audio_stream::get_channels() const
	{
		if (m_stream) return al_get_audio_stream_channels(m_stream);
		return ALLEGRO_CHANNEL_CONF();
	}

	ALLEGRO_AUDIO_DEPTH Audio_stream::get_depth() const
	{
		if (m_stream) return al_get_audio_stream_depth(m_stream);
		return ALLEGRO_AUDIO_DEPTH();
	}

	unsigned Audio_stream::get_frequency() const
	{
		if (m_stream) return al_get_audio_stream_frequency(m_stream);
		return 0;
	}

	unsigned Audio_stream::get_length() const
	{
		if (m_stream) return al_get_audio_stream_length(m_stream);
		return 0;
	}

	float Audio_stream::get_speed() const
	{
		if (m_stream) return al_get_audio_stream_speed(m_stream);
		return 0.0f;
	}

	float Audio_stream::get_gain() const
	{
		if (m_stream) return al_get_audio_stream_gain(m_stream);
		return 0.0f;
	}

	float Audio_stream::get_pan() const
	{
		if (m_stream) return al_get_audio_stream_pan(m_stream);
		return 0.0f;
	}

	ALLEGRO_PLAYMODE Audio_stream::get_playmode() const
	{
		if (m_stream) return al_get_audio_stream_playmode(m_stream);
		return ALLEGRO_PLAYMODE();
	}

	bool Audio_stream::get_attached() const
	{
		if (m_stream) return al_get_audio_stream_attached(m_stream);
		return false;
	}

	uint64_t Audio_stream::get_played_samples() const
	{
		if (m_stream) return al_get_audio_stream_played_samples(m_stream);
		return 0;
	}

	unsigned Audio_stream::get_fragments() const
	{
		if (m_stream) return al_get_audio_stream_fragments(m_stream);
		return 0;
	}

	unsigned Audio_stream::get_available_fragments() const
	{
		if (m_stream) return al_get_available_audio_stream_fragments(m_stream);
		return 0;
	}
	bool Audio_stream::set_speed(const float speed)
	{
		if (m_stream) return al_set_audio_stream_speed(m_stream, speed);
		return false;
	}

	bool Audio_stream::set_gain(const float gain)
	{
		if (m_stream) return al_set_audio_stream_gain(m_stream, gain);
		return false;
	}

	bool Audio_stream::set_pan(const float pan)
	{
		if (m_stream) return al_set_audio_stream_pan(m_stream, pan);
		return false;
	}

	bool Audio_stream::set_playmode(const ALLEGRO_PLAYMODE pm)
	{
		if (m_stream) return al_set_audio_stream_playmode(m_stream, pm);
		return false;
	}

	bool Audio_stream::set_playing(const bool pl)
	{
		if (m_stream) return al_set_audio_stream_playing(m_stream, pl);
		return false;
	}

	bool Audio_stream::set_loop_secs(const double beg, const double end)
	{
		if (end <= beg) return false;
		if (m_stream) return al_set_audio_stream_loop_secs(m_stream, beg, end);
		return false;
	}

	double Audio_stream::get_position_secs() const
	{
		if (m_stream) return al_get_audio_stream_position_secs(m_stream);
		return 0.0;
	}

	double Audio_stream::get_length_secs() const
	{
		if (m_stream) return al_get_audio_stream_length_secs(m_stream);
		return 0.0;
	}

	bool Audio_stream::seek_secs(const double tim)
	{
		if (m_stream) return al_seek_audio_stream_secs(m_stream, tim);
		return false;
	}

	void* Audio_stream::get_fragment()
	{
		if (m_stream) return al_get_audio_stream_fragment(m_stream);
		return nullptr;
	}

	bool Audio_stream::set_fragment(void* arg)
	{
		if (m_stream) return al_set_audio_stream_fragment(m_stream, arg);
		return false;
	}

	bool Audio_stream::handle_fragment(std::function<bool(void*)> f)
	{
		if (!f) return false;
		void* _dat = get_fragment();
		if (!_dat) return false;
		if (f(_dat)) {
			return set_fragment(_dat);
		}
		return false;
	}

	ALLEGRO_AUDIO_STREAM* Audio_stream::get() const
	{
		return m_stream;
	}

	Audio_stream::operator ALLEGRO_AUDIO_STREAM* () const
	{
		return m_stream;
	}
}