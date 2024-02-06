#include "../include/audio_mixer.h"

#include <stdexcept>

namespace AllegroCPP {

	extern void __audio_allegro_start(int);

	Mixer::Mixer(const bool start, const int sp)
	{
		if (start) __audio_allegro_start(sp);
	}

	Mixer::Mixer(const int samplestart, const unsigned freq, const ALLEGRO_AUDIO_DEPTH depth, const ALLEGRO_CHANNEL_CONF conf)
	{
		__audio_allegro_start(samplestart);
		if (!create(freq, depth, conf)) throw std::invalid_argument("Could not create Mixer.");
	}

	Mixer::Mixer(Mixer&& oth)
		: m_mixer(std::exchange(oth.m_mixer, nullptr))
	{
	}

	void Mixer::operator=(Mixer&& oth)
	{
		destroy();
		m_mixer = std::exchange(oth.m_mixer, nullptr);
	}

	Mixer::~Mixer()
	{
		destroy();
	}

	bool Mixer::create(const unsigned freq, const ALLEGRO_AUDIO_DEPTH depth, const ALLEGRO_CHANNEL_CONF conf)
	{
		destroy();
		return (m_mixer = al_create_mixer(freq, depth, conf));
	}

	void Mixer::destroy()
	{
		if (m_mixer) {
			al_destroy_mixer(m_mixer);
			m_mixer = nullptr;
		}
	}

	void Mixer::detach()
	{
		if (m_mixer) al_detach_mixer(m_mixer);
	}

	bool Mixer::attach_audio_stream(ALLEGRO_AUDIO_STREAM* oth)
	{
		if (m_mixer) return al_attach_audio_stream_to_mixer(oth, m_mixer);
		return false;
	}

	bool Mixer::attach_mixer(ALLEGRO_MIXER* oth)
	{
		if (m_mixer) return al_attach_mixer_to_mixer(oth, m_mixer);
		return false;
	}

	bool Mixer::attach_sample_instance(ALLEGRO_SAMPLE_INSTANCE* oth)
	{
		if (m_mixer) return al_attach_sample_instance_to_mixer(oth, m_mixer);
		return false;
	}

	Mixer& Mixer::operator<<(ALLEGRO_AUDIO_STREAM* oth)
	{
		attach_audio_stream(oth);
		return *this;
	}

	Mixer& Mixer::operator<<(ALLEGRO_MIXER* oth)
	{
		attach_mixer(oth);
		return *this;
	}

	Mixer& Mixer::operator<<(ALLEGRO_SAMPLE_INSTANCE* oth)
	{
		attach_sample_instance(oth);
		return *this;
	}

	unsigned Mixer::get_frequency() const
	{
		if (m_mixer) return al_get_mixer_frequency(m_mixer);
		return 0;
	}

	ALLEGRO_CHANNEL_CONF Mixer::get_channels() const
	{
		if (m_mixer) return al_get_mixer_channels(m_mixer);
		return ALLEGRO_CHANNEL_CONF();
	}

	ALLEGRO_AUDIO_DEPTH Mixer::get_depth() const
	{
		if (m_mixer) return al_get_mixer_depth(m_mixer);
		return ALLEGRO_AUDIO_DEPTH();
	}

	float Mixer::get_gain() const
	{
		if (m_mixer) return al_get_mixer_gain(m_mixer);
		return 0.0f;
	}

	ALLEGRO_MIXER_QUALITY Mixer::get_quality() const
	{
		if (m_mixer) return al_get_mixer_quality(m_mixer);
		return ALLEGRO_MIXER_QUALITY();
	}

	bool Mixer::get_playing() const
	{
		if (m_mixer) return al_get_mixer_playing(m_mixer);
		return false;
	}

	bool Mixer::get_attached() const
	{
		if (m_mixer) return al_get_mixer_attached(m_mixer);
		return false;
	}

	bool Mixer::set_frequency(const unsigned freq)
	{
		if (m_mixer) return al_set_mixer_frequency(m_mixer, freq);
		return false;
	}

	bool Mixer::set_gain(const float gain)
	{
		if (m_mixer) return al_set_mixer_gain(m_mixer, gain);
		return false;
	}

	bool Mixer::set_quality(const ALLEGRO_MIXER_QUALITY qual)
	{
		if (m_mixer) return al_set_mixer_quality(m_mixer, qual);
		return false;
	}

	bool Mixer::set_playing(const bool playing)
	{
		if (m_mixer) return al_set_mixer_playing(m_mixer, playing);
		return false;
	}

	bool Mixer::set_postprocess_callback(void (*cb)(void*, unsigned int, void*), void* data)
	{
		if (m_mixer) return al_set_mixer_postprocess_callback(m_mixer, cb, data);
		return false;
	}

	ALLEGRO_MIXER* Mixer::get() const
	{
		return m_mixer;
	}

	Mixer::operator ALLEGRO_MIXER* () const
	{
		return m_mixer;
	}

	Default_Mixer::Default_Mixer(const int samplestart)
	{
		__audio_allegro_start(samplestart);
		if (!(m_mixer = al_get_default_mixer())) {
			if (!al_restore_default_mixer()) throw std::runtime_error("Cannot create/recreate default mixer.");
			if (!(m_mixer = al_get_default_mixer())) throw std::runtime_error("Default mixer still null after recreation.");
		}
	}

	Default_Mixer::~Default_Mixer()
	{
		m_mixer = nullptr; // don't touch it.
	}

	void Default_Mixer::set_default(Mixer&& oth)
	{
		al_set_default_mixer(std::exchange(oth.m_mixer, nullptr));
	}

	void Default_Mixer::destroy_default()
	{
		al_set_default_mixer(nullptr);
	}

}