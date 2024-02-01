#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>

#include "../include/audio_sample.h"

namespace AllegroCPP {

	class Sample_instance {
		Sample_shared_wptr m_sampl;
		ALLEGRO_SAMPLE_INSTANCE* m_sampl_inst = nullptr;

		Sample_shared_sptr _lock() const;
	public:
		Sample_instance(const Sample_instance&) = delete;
		void operator=(const Sample_instance&) = delete;

		Sample_instance(const Sample&);
		Sample_instance(Sample_instance&&);
		void operator=(Sample_instance&&);
		~Sample_instance();

		bool create(const Sample&);
		bool create_duplicate(const Sample_instance&);

		const Sample& operator<<(const Sample&);

		void destroy();
		void detach() const;

		bool play();
		bool stop();

		ALLEGRO_CHANNEL_CONF get_channels() const;
		ALLEGRO_AUDIO_DEPTH get_depth() const;
		unsigned get_frequency() const;
		unsigned get_length() const;
		unsigned get_position() const;
		float get_speed() const;
		float get_gain() const;
		float get_pan() const;
		float get_time() const;
		ALLEGRO_PLAYMODE get_playmode() const;
		bool get_playing() const;
		bool get_attached() const;

		bool set_length(const unsigned);
		bool set_position(const unsigned);
		bool set_speed(const float);
		bool set_gain(const float);
		bool set_pan(const float = ALLEGRO_AUDIO_PAN_NONE);
		bool set_playmode(const ALLEGRO_PLAYMODE);
		bool set_playing(const bool);

		Sample_shared_wptr get_sample() const;

		ALLEGRO_SAMPLE_INSTANCE* get() const;
		operator ALLEGRO_SAMPLE_INSTANCE*() const;
	};

}