#pragma once

#include <allegro5/allegro.h>
#include <utility>

namespace AllegroCPP {

	class Timer {
		ALLEGRO_TIMER* m_tim = nullptr;
	public:
		Timer(const Timer&) = delete;
		Timer(Timer&&) noexcept;
		void operator=(const Timer&) = delete;
		void operator=(Timer&&) noexcept;
		
		Timer(const double);

		void start();
		void stop();
		bool started() const;
		int64_t count() const; // ticks
		void set_count(const int64_t);
		void add_count(const int64_t);
		double get_speed() const;
		void set_speed(const double);

		operator ALLEGRO_TIMER* ();
		operator ALLEGRO_EVENT_SOURCE* ();
		operator bool() const;
	};

}