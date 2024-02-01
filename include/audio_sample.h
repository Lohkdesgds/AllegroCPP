#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>

#include <string>

#include "../include/file.h"

namespace AllegroCPP {
	
	using Sample_shared_sptr = std::shared_ptr<ALLEGRO_SAMPLE>;
	using Sample_shared_wptr = std::weak_ptr<ALLEGRO_SAMPLE>;

	class Sample {
		Sample_shared_sptr m_sampl;
		File_shareable_ptr m_fileload;
	public:
		Sample(const Sample&) = delete;
		void operator=(const Sample&) = delete;

		Sample(Sample&&);
		void operator=(Sample&&);
		Sample(const std::string&);
		Sample(File_shareable_ptr);
		Sample(File_shareable_ptr, const std::string&);
		Sample(unsigned, const unsigned, ALLEGRO_AUDIO_DEPTH, ALLEGRO_CHANNEL_CONF);

		bool load(const std::string&);
		bool load(File_shareable_ptr);
		bool load(File_shareable_ptr, const std::string&);
		void destroy();

		bool create(unsigned, const unsigned = 48000, ALLEGRO_AUDIO_DEPTH = ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF = ALLEGRO_CHANNEL_CONF_2);

		bool save(const std::string&);
		bool save(File_shareable_ptr);
		bool save(File_shareable_ptr, const std::string&);

		ALLEGRO_CHANNEL_CONF get_channels() const;
		ALLEGRO_AUDIO_DEPTH get_depth() const;
		unsigned get_frequency() const;
		unsigned get_length() const;
		void* get_data();

		ALLEGRO_SAMPLE* get() const;
		operator ALLEGRO_SAMPLE* () const;
		operator Sample_shared_wptr() const;
	};

}