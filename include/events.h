#pragma once

#include <allegro5/allegro.h>

#include <memory>
#include <functional>
#include <any>
#include <stdexcept>
#include <atomic>

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

	class Event_custom {
		//class any_sized : public std::any {
		//	size_t m_siz = 0;
		//public:
		//	any_sized(const any_sized& oth);
		//	any_sized(any_sized&& oth);
		//	template<typename T> any_sized(T cpy) : std::any(cpy), m_siz(sizeof(cpy)) {}
		//	template<typename T> any_sized& operator=(T cpy) { this->std::any::operator=(std::move(cpy)); m_siz = sizeof(cpy); }
		//	any_sized& operator=(const any_sized&) noexcept;
		//	void operator=(any_sized&&) noexcept;
		//
		//	std::any& get();
		//	const std::any& get() const;
		//	size_t size() const;
		//};
		
		ALLEGRO_EVENT_SOURCE* m_ev = nullptr;
		std::atomic<size_t> accumulated_events = 0;
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
				
		bool emit(const char* val, int id, std::function<void(void)> trigger_on_del = {});
		bool emit(const std::string& val, int id, std::function<void(void)> trigger_on_del = {});
		bool emit(const std::vector<char>& val, int id, std::function<void(void)> trigger_on_del = {});
		bool emit(std::any val, int id, std::function<void(void)> trigger_on_del = {});
		bool emit(void* data, std::function<void(void*)> dtor, int id, std::function<void(void)> trigger_on_del = {});
		bool emit(ALLEGRO_EVENT, void (*dtor)(ALLEGRO_USER_EVENT*) = nullptr);

		size_t total_on_queue() const;

		// on emit, if dtor != null, do unref()!
		//bool emit(ALLEGRO_EVENT, void(*dtor)(ALLEGRO_USER_EVENT*) = nullptr);
		//static void unref(ALLEGRO_USER_EVENT);
		//static void unref(ALLEGRO_USER_EVENT*);

		//void* get_data();
		//bool set_data(void*);
	};

	//template<typename T, std::enable_if_t<std::is_pointer_v<std::remove_cvref_t<T>>, int>>
	//inline bool AllegroCPP::Event_custom::post(T val, int id, std::function<void(void)> trigger_on_del)
	//{
	//	if (!m_ev) return false;
	//	if (!ALLEGRO_EVENT_TYPE_IS_USER(id)) throw std::invalid_argument("ID must be a USER_TYPE type to work (must follow macro ALLEGRO_EVENT_TYPE_IS_USER(X))");
	//
	//	T* newt = new T(std::move(val));
	//	std::function<void(void*)>* _f = new std::function<void(void*)>([](void* ptr) {std::default_delete<T>(ptr); });
	//	std::function<void(void)>* _tck = new std::function<void(void)>(trigger_on_del);
	//
	//	ALLEGRO_EVENT ev{};
	//	ev.user.data1 = (intptr_t)newt;
	//	ev.user.data2 = (intptr_t)_f;
	//	ev.user.data3 = (intptr_t)_tck;
	//	ev.user.data4 = (intptr_t)&accumulated_events;
	//	ev.user.type = id;
	//
	//	++accumulated_events;
	//	al_emit_user_event(m_ev, &ev, &__event_custom_dtor);
	//}

	void __event_custom_dtor(ALLEGRO_USER_EVENT*);
}