#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>

namespace AllegroCPP {

	class Default_Voice;

	class Voice {
	protected:
		ALLEGRO_VOICE* m_voice = nullptr;

		friend class Default_Voice;
	public:
		Voice(bool, int = 0);
		Voice(int = 0, unsigned = 48000, ALLEGRO_AUDIO_DEPTH = ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF = ALLEGRO_CHANNEL_CONF_2);
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

		bool set_playing(const bool);

		// non-streaming object attached, this returns current sample pos
		unsigned get_position() const;
		// non-streaming object attached, this set current sample pos
		bool set_position(const unsigned);

		ALLEGRO_VOICE* get() const;
		operator ALLEGRO_VOICE*() const;
	};

	class Default_Voice : public Voice {
		using Voice::create;
		using Voice::destroy;
	public:
		Default_Voice(int = 0);
		~Default_Voice();

		void set_default(Voice&&);
		void destroy_default();
	};

}