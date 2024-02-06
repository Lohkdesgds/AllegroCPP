#pragma once

#include <allegro5/allegro.h>

#include <memory>
#include <functional>
#include <any>
#include <stdexcept>
#include <atomic>
#include <string>

namespace AllegroCPP
{
	class Event_keyboard {
	public:
		Event_keyboard();
		operator ALLEGRO_EVENT_SOURCE* () const;
	};

	class Event_mouse {
	public:
		Event_mouse();
		operator ALLEGRO_EVENT_SOURCE* () const;
	};

	class Event_touch {
	public:
		Event_touch();
		operator ALLEGRO_EVENT_SOURCE* () const;
	};

	class Event_joystick {
	public:
		Event_joystick();
		operator ALLEGRO_EVENT_SOURCE* () const;
	};

	class Event_custom {		
		ALLEGRO_EVENT_SOURCE* m_ev = nullptr;
		std::atomic<size_t> m_accumulated_events = 0;
	public:
		Event_custom();
		~Event_custom();

		Event_custom(const Event_custom&) = delete;
		Event_custom(Event_custom&&) noexcept;
		void operator=(const Event_custom&) = delete;
		void operator=(Event_custom&&) noexcept;

		operator ALLEGRO_EVENT_SOURCE* () const;

		bool set_data(std::any&& any);
		bool has_data() const;
		std::any get_data() const;
		bool clear_data();
				
		bool emit(const char* val, const int id, std::function<void(void)> trigger_on_del = {});
		bool emit(const std::string& val, const int id, std::function<void(void)> trigger_on_del = {});
		bool emit(const std::vector<char>& val, const int id, std::function<void(void)> trigger_on_del = {});
		bool emit(std::any val, const int id, std::function<void(void)> trigger_on_del = {});
		bool emit(void* data, std::function<void(void*)> dtor, const int id, std::function<void(void)> trigger_on_del = {});

		// vanilla style
		bool emit(ALLEGRO_EVENT, void (*dtor)(ALLEGRO_USER_EVENT*) = nullptr);

		// entries allocated in queue
		size_t total_on_queue() const;
	};

	// user event has raw ptr, destructor ptr, trace function ptr and counter as data1..4
	void __event_custom_dtor(ALLEGRO_USER_EVENT*);
}