#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>

#include <functional>

#include "../include/file.h"

namespace AllegroCPP {

	class Audio_stream {
		ALLEGRO_AUDIO_STREAM* m_stream = nullptr;
		File_shareable_ptr m_fileload;
	public:
		Audio_stream(const Audio_stream&) = delete;
		void operator=(const Audio_stream&) = delete;

		Audio_stream(Audio_stream&&);
		void operator=(Audio_stream&&);
		Audio_stream(const std::string&, const size_t = 4, const unsigned = 2048);
		Audio_stream(File_shareable_ptr, const size_t = 4, const unsigned = 2048);
		Audio_stream(File_shareable_ptr, const std::string&, const size_t = 4, const unsigned = 2048);
		Audio_stream(const size_t, const unsigned, const unsigned, ALLEGRO_AUDIO_DEPTH, ALLEGRO_CHANNEL_CONF);

		bool load(const std::string&, const size_t = 4, const unsigned = 2048);
		bool load(File_shareable_ptr, const size_t = 4, const unsigned = 2048);
		bool load(File_shareable_ptr, const std::string&, const size_t = 4, const unsigned = 2048);

		bool create(const size_t, const unsigned, const unsigned, ALLEGRO_AUDIO_DEPTH, ALLEGRO_CHANNEL_CONF);

		ALLEGRO_EVENT_SOURCE* get_event_source() const;
		operator ALLEGRO_EVENT_SOURCE* () const;

		// wait the buffer to flush completely (end things)
		void drain() const;
		// rewind back to start (valid on file streams)
		bool rewind();

		void destroy();
		bool detach() const;

		ALLEGRO_CHANNEL_CONF get_channels() const;
		ALLEGRO_AUDIO_DEPTH get_depth() const;
		unsigned get_frequency() const;
		unsigned get_length() const;
		float get_speed() const;
		float get_gain() const;
		float get_pan() const;
		ALLEGRO_PLAYMODE get_playmode() const;
		bool get_attached() const;

		uint64_t get_played_samples() const;
		unsigned get_fragments() const;
		unsigned get_available_fragments() const;

		bool set_speed(const float);
		bool set_gain(const float);
		bool set_pan(const float = ALLEGRO_AUDIO_PAN_NONE);
		bool set_playmode(const ALLEGRO_PLAYMODE);
		bool set_playing(const bool);

		bool set_loop_secs(const double, const double);

		double get_position_secs() const;
		double get_length_secs() const;
		bool seek_secs(const double);

		// returns non null if stream is ready for new data.
		void* get_fragment();
		// send back the get_fragment() data using this.
		bool set_fragment(void*);

		// get_fragment is called. If true, function is called. If function returns true, set_fragment is called. Returns true if all returns true.
		bool handle_fragment(std::function<bool(void*)>);

		ALLEGRO_AUDIO_STREAM* get() const;
		operator ALLEGRO_AUDIO_STREAM* () const;
	};

}