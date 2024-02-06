#include "../include/audio_voice.h"


#include <stdexcept>

namespace AllegroCPP {

	extern void __audio_allegro_start(int);

	Voice::Voice(const bool start, const int sp)
	{
		if (start) __audio_allegro_start(sp);
	}

	Voice::Voice(const int samplestart, const unsigned freq, const ALLEGRO_AUDIO_DEPTH depth, const ALLEGRO_CHANNEL_CONF conf)
	{
		__audio_allegro_start(samplestart);
		if (!create(freq, depth, conf)) throw std::invalid_argument("Could not create Voice.");
	}

	Voice::Voice(Voice&& oth)
		: m_voice(std::exchange(oth.m_voice, nullptr))
	{
	}

	void Voice::operator=(Voice&& oth)
	{
		destroy();
		m_voice = std::exchange(oth.m_voice, nullptr);
	}

	Voice::~Voice()
	{
		destroy();
	}

	bool Voice::create(const unsigned freq, const ALLEGRO_AUDIO_DEPTH depth, const ALLEGRO_CHANNEL_CONF conf)
	{
		destroy();
		return (m_voice = al_create_voice(freq, depth, conf));
	}

	void Voice::destroy()
	{
		if (m_voice) {
			al_destroy_voice(m_voice);
			m_voice = nullptr;
		}
	}

	void Voice::detach()
	{
		if (m_voice) al_detach_voice(m_voice);
	}

	bool Voice::attach_audio_stream(ALLEGRO_AUDIO_STREAM* oth)
	{
		if (m_voice) return al_attach_audio_stream_to_voice(oth, m_voice);
		return false;
	}

	bool Voice::attach_mixer(ALLEGRO_MIXER* oth)
	{
		if (m_voice) return al_attach_mixer_to_voice(oth, m_voice);
		return false;
	}

	bool Voice::attach_sample_instance(ALLEGRO_SAMPLE_INSTANCE* oth)
	{
		if (m_voice) return al_attach_sample_instance_to_voice(oth, m_voice);
		return false;
	}

	Voice& Voice::operator<<(ALLEGRO_AUDIO_STREAM* oth)
	{
		attach_audio_stream(oth);
		return *this;
	}

	Voice& Voice::operator<<(ALLEGRO_MIXER* oth)
	{
		attach_mixer(oth);
		return *this;
	}

	Voice& Voice::operator<<(ALLEGRO_SAMPLE_INSTANCE* oth)
	{
		attach_sample_instance(oth);
		return *this;
	}

	unsigned Voice::get_frequency() const
	{
		if (m_voice) return al_get_voice_frequency(m_voice);
		return 0;
	}

	ALLEGRO_CHANNEL_CONF Voice::get_channels() const
	{
		if (m_voice) return al_get_voice_channels(m_voice);
		return static_cast<ALLEGRO_CHANNEL_CONF>(0);
	}

	ALLEGRO_AUDIO_DEPTH Voice::get_depth() const
	{
		if (m_voice) return al_get_voice_depth(m_voice);
		return static_cast<ALLEGRO_AUDIO_DEPTH>(0);
	}

	bool Voice::get_playing() const
	{
		if (m_voice) return al_get_voice_playing(m_voice);
		return false;
	}

	bool Voice::set_playing(const bool s)
	{
		if (m_voice) return al_set_voice_playing(m_voice, s);
		return false;
	}

	unsigned Voice::get_position() const
	{
		if (m_voice) return al_get_voice_position(m_voice);
		return 0;
	}

	bool Voice::set_position(const unsigned p)
	{
		if (m_voice) return al_set_voice_position(m_voice, p);
		return false;
	}

	ALLEGRO_VOICE* Voice::get() const
	{
		return m_voice;
	}

	Voice::operator ALLEGRO_VOICE* () const
	{
		return m_voice;
	}
		
	Default_Voice::Default_Voice(const int samplestart)
	{
		__audio_allegro_start(samplestart);
		if (!(m_voice = al_get_default_voice())) {
			if (!al_restore_default_mixer()) throw std::runtime_error("Cannot create/recreate default voice.");
			if (!(m_voice = al_get_default_voice())) throw std::runtime_error("Default voice still null after recreation.");
		}
	}

	Default_Voice::~Default_Voice()
	{
		m_voice = nullptr; // don't touch it.
	}

	void Default_Voice::set_default(Voice&& oth)
	{
		al_set_default_voice(std::exchange(oth.m_voice, nullptr));
	}

	void Default_Voice::destroy_default()
	{
		al_set_default_voice(nullptr);
	}

}