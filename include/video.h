#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_video.h>

#include "bitmap.h"

#include <string>

namespace AllegroCPP {

	class Video;

	// Just to remember that VIDEO has ALLEGRO_VIDEO on first element
	class Video_event {
		const ALLEGRO_EVENT m_ev{};
	public:
		Video_event() = delete;
		Video_event(Video_event&&) = delete;
		void operator=(Video_event&&) = delete;
		Video_event(const Video_event&) = delete;
		void operator=(const Video_event&) = delete;

		Video_event(const ALLEGRO_EVENT);

		ALLEGRO_VIDEO* get_source() const;

		// as const, won't destroy source.
		Video get_video_source() const;
	};

	class Video : protected Bitmap {
		ALLEGRO_VIDEO* m_video = nullptr;
		const bool m_delete_auto = true;

		ALLEGRO_BITMAP* get_for_draw() const override;
	public:
		enum dir {SET, CUR}; // END not supported

		Video(const Video&) = delete;
		void operator=(const Video&) = delete;

		Video(Video&&) noexcept;
		void operator=(Video&&) noexcept;

		Video();
		Video(const std::string&);
		// Won't delete video, just ref
		Video(ALLEGRO_VIDEO*);
		~Video();

		bool start(ALLEGRO_MIXER*);
		bool start(ALLEGRO_VOICE*);

		void destroy();

		void set_playing(const bool);
		bool is_playing() const;

		double get_audio_rate() const;
		double get_fps() const;

		float get_scaled_width() const;
		float get_scaled_height() const;

		// do not free. Valid until next get_frame or destroy
		ALLEGRO_BITMAP* get_frame() const;
		operator ALLEGRO_BITMAP*() const;

		double get_position(const ALLEGRO_VIDEO_POSITION_TYPE = ALLEGRO_VIDEO_POSITION_ACTUAL) const;
		
		bool seek(const double, const dir = dir::SET);

		ALLEGRO_EVENT_SOURCE* get_event_source() const;
		operator ALLEGRO_EVENT_SOURCE* () const;

		bool operator==(const ALLEGRO_VIDEO*) const;
		bool operator!=(const ALLEGRO_VIDEO*) const;

		static std::string identify(const std::string&);
		static std::string identify_f(ALLEGRO_FILE*);

		using Bitmap::get_flags;
		using Bitmap::get_format;
		using Bitmap::get_width;
		using Bitmap::get_height;
		using Bitmap::get_pixel;
		using Bitmap::draw;
	};

}