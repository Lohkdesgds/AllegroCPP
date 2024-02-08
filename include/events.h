#pragma once

#include <allegro5/allegro.h>

#include <memory>
#include <functional>
#include <any>
#include <stdexcept>
#include <atomic>
#include <string>

#ifdef _WIN32
#include <allegro5/allegro_windows.h>
#endif

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

#ifdef _WIN32
	// data1 = string (char*) of path
	class Event_drag_and_drop : private Event_custom {
		HWND m_hwnd = nullptr;
		WNDPROC m_raw_proc_old = nullptr;
		const int m_custom_id;

		friend LRESULT CALLBACK __event_dnd_raw_wndProc(HWND, UINT, WPARAM, LPARAM);
		LRESULT CALLBACK __event_dnd_wndProc(HWND, UINT, WPARAM, LPARAM);
	public:
		Event_drag_and_drop(const Event_drag_and_drop&) = delete;
		Event_drag_and_drop(Event_drag_and_drop&&) = delete;
		void operator=(const Event_drag_and_drop&) = delete;
		void operator=(Event_drag_and_drop&&) = delete;

		// Enables on display the event
		Event_drag_and_drop(ALLEGRO_DISPLAY*, const int custom_id = 1024);
		// Disable drag and drop event
		~Event_drag_and_drop();

		using Event_custom::operator ALLEGRO_EVENT_SOURCE*;
	};

	class Drop_event {
		const std::string m_copy_string;
	public:
		Drop_event(const Drop_event&) = delete;
		Drop_event(Drop_event&&) = delete;
		void operator=(const Drop_event&) = delete;
		void operator=(Drop_event&&) = delete;

		Drop_event(const ALLEGRO_EVENT&);

		const std::string& c_str();
	};
#endif

}