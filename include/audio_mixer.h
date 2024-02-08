#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>

namespace AllegroCPP {

	class Default_Mixer;

	class Mixer {
	protected:
		ALLEGRO_MIXER* m_mixer = nullptr;

		friend class Default_Mixer;
	public:
		Mixer(const bool, const int = 0);
		Mixer(const int = 0, const unsigned = 48000, const ALLEGRO_AUDIO_DEPTH = ALLEGRO_AUDIO_DEPTH_INT16, const ALLEGRO_CHANNEL_CONF = ALLEGRO_CHANNEL_CONF_2);
		Mixer(const Mixer&) = delete;
		void operator=(const Mixer&) = delete;
		Mixer(Mixer&&);
		void operator=(Mixer&&);
		~Mixer();

		bool create(const unsigned, const ALLEGRO_AUDIO_DEPTH = ALLEGRO_AUDIO_DEPTH_INT16, const ALLEGRO_CHANNEL_CONF = ALLEGRO_CHANNEL_CONF_2);
		void destroy();

		void detach();

		bool attach_audio_stream(ALLEGRO_AUDIO_STREAM*);
		bool attach_mixer(ALLEGRO_MIXER*);
		bool attach_sample_instance(ALLEGRO_SAMPLE_INSTANCE*);

		Mixer& operator<<(ALLEGRO_AUDIO_STREAM*);
		Mixer& operator<<(ALLEGRO_MIXER*);
		Mixer& operator<<(ALLEGRO_SAMPLE_INSTANCE*);

		unsigned get_frequency() const;
		ALLEGRO_CHANNEL_CONF get_channels() const;
		ALLEGRO_AUDIO_DEPTH get_depth() const;
		float get_gain() const;
		ALLEGRO_MIXER_QUALITY get_quality() const;
		bool get_playing() const;
		bool get_attached() const;

		// only works if not attached to anything
		bool set_frequency(const unsigned);
		bool set_gain(const float);
		bool set_quality(const ALLEGRO_MIXER_QUALITY);
		bool set_playing(const bool);
		bool set_postprocess_callback(void (*)(void*, unsigned int, void*), void*);

		ALLEGRO_MIXER* get() const;
		operator ALLEGRO_MIXER* () const;
	};

	class Default_Mixer : public Mixer {
		using Mixer::create;
		using Mixer::destroy;
	public:
		Default_Mixer(const int = 0);
		~Default_Mixer();

		void set_default(Mixer&&);
		void destroy_default();
	};
}