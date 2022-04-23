#pragma once

#include <allegro5/allegro.h>

#include <stdexcept>
#include <any>

namespace AllegroCPP {

	class Event {
		bool valid_event = false;
		ALLEGRO_EVENT ev;
	public:
		Event(Event&&) noexcept;
		Event(const ALLEGRO_EVENT&, const bool);
		~Event();

		operator bool() const;
		void operator=(const Event&) = delete;
		void operator=(Event&&) noexcept;

		bool empty() const;
		bool valid() const;

		ALLEGRO_EVENT get() const;

		const std::any* get_data();
		bool replace_data(std::any);
		bool clear_data();
	};

	class Event_queue {
		ALLEGRO_EVENT_QUEUE* m_evq = nullptr;
	public:
		Event_queue();
		~Event_queue();

		Event_queue(const Event_queue&) = delete;
		Event_queue(Event_queue&&) noexcept;
		void operator=(const Event_queue&) = delete;
		void operator=(Event_queue&&) noexcept;

		bool valid() const;

		operator bool() const;

		bool register_source(ALLEGRO_EVENT_SOURCE*);
		Event_queue& operator<<(ALLEGRO_EVENT_SOURCE*);

		bool remove_source(ALLEGRO_EVENT_SOURCE*);

		void pause();
		bool is_paused() const;

		bool is_queue_empty() const;
		bool has_event() const;

		Event get_next_event();
		Event peek_next_event();
		bool drop_next_event();
		void flush_event_queue();
		Event wait_for_event(float secs = -1.f);
		Event wait_for_event(ALLEGRO_TIMEOUT* until);
	};

}