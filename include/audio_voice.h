#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>

#include <stdexcept>

namespace AllegroCPP {

	constexpr int default_voice_audio_samples = 16;

	class Voice {
		ALLEGRO_VOICE* m_voice = nullptr;
	public:
		Voice(int = default_voice_audio_samples);
		Voice(const Voice&) = delete;
		void operator=(const Voice&) = delete;
		Voice(Voice&&);
		void operator=(Voice&&);
		~Voice();

		bool create(unsigned, ALLEGRO_AUDIO_DEPTH = ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF = ALLEGRO_CHANNEL_CONF_2);
		void destroy();

		void detach();

		bool attach_audio_stream(ALLEGRO_AUDIO_STREAM*);
		bool attach_mixer(ALLEGRO_MIXER*);
		bool attach_sample_instance(ALLEGRO_SAMPLE_INSTANCE*);

		Voice& operator<<(ALLEGRO_AUDIO_STREAM*);
		Voice& operator<<(ALLEGRO_MIXER*);
		Voice& operator<<(ALLEGRO_SAMPLE_INSTANCE*);
		
		unsigned get_frequency() const;
		ALLEGRO_CHANNEL_CONF get_channels() const;
		ALLEGRO_AUDIO_DEPTH get_depth() const;
		bool get_playing() const;

		void set_playing(const bool);

		// non-streaming object attached, this returns current sample pos
		unsigned get_position() const;
		// non-streaming object attached, this set current sample pos
		void set_position(const unsigned);

		ALLEGRO_VOICE* get() const;
		operator ALLEGRO_VOICE*() const;
	};

}