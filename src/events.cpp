#include "../include/events.h"

namespace AllegroCPP {

	Event_keyboard::Event_keyboard()
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_keyboard_installed()) al_install_keyboard();
	}

	Event_keyboard::operator ALLEGRO_EVENT_SOURCE* () const
	{
		return al_get_keyboard_event_source();
	}


	Event_mouse::Event_mouse()
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_mouse_installed()) al_install_mouse();
	}

	Event_mouse::operator ALLEGRO_EVENT_SOURCE* () const
	{
		return al_get_mouse_event_source();
	}


	Event_touch::Event_touch()
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_touch_input_installed()) al_install_touch_input();
	}

	Event_touch::operator ALLEGRO_EVENT_SOURCE* () const
	{
		return al_get_touch_input_event_source();
	}


	Event_joystick::Event_joystick()
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_joystick_installed()) al_install_joystick();
	}

	Event_joystick::operator ALLEGRO_EVENT_SOURCE* () const
	{
		return al_get_joystick_event_source();
	}


	Event_custom::Event_custom()
		: m_ev(new ALLEGRO_EVENT_SOURCE())
	{
		if (!al_is_system_installed()) al_init();
		al_init_user_event_source(m_ev);
	}

	Event_custom::~Event_custom()
	{
		if (m_ev) {
			clear_data();
			al_destroy_user_event_source(m_ev);
			delete m_ev;
			m_ev = nullptr;
			ALLEGRO_ASSERT(m_accumulated_events == 0); // if not 0 there may be a memory leak! (Events are queued and they were not deleted properly. This variable is the amount queued. It should be ZERO)
		}
	}

	Event_custom::Event_custom(Event_custom&& oth) noexcept
		: m_ev(oth.m_ev), m_accumulated_events(oth.m_accumulated_events.exchange(0))
	{
		oth.m_ev = nullptr;
	}

	void Event_custom::operator=(Event_custom&& oth) noexcept
	{
		if (m_ev) {
			clear_data();
			al_destroy_user_event_source(m_ev);
			delete m_ev;
			ALLEGRO_ASSERT(m_accumulated_events == 0); // if not 0 there may be a memory leak! (Events are queued and they were not deleted properly. This variable is the amount queued. It should be ZERO)
		}
		m_accumulated_events = oth.m_accumulated_events.exchange(0);
		m_ev = oth.m_ev;
		oth.m_ev = nullptr;
	}

	Event_custom::operator ALLEGRO_EVENT_SOURCE* () const
	{
		return m_ev;
	}

	bool Event_custom::set_data(std::any&& any)
	{
		if (!m_ev) return false;
		al_set_event_source_data(m_ev, (intptr_t)(new std::any(std::move(any))));
		return true;
	}

	bool Event_custom::has_data() const
	{
		if (!m_ev) return false;
		return al_get_event_source_data(m_ev) != 0;
	}

	std::any Event_custom::get_data() const
	{
		if (!m_ev) return {};
		std::any* ptr = (std::any*)al_get_event_source_data(m_ev);
		if (!ptr) return {};
		return *ptr;
	}

	bool Event_custom::clear_data()
	{
		if (!m_ev) return true;
		std::any* ptr = (std::any*)al_get_event_source_data(m_ev);
		if (ptr) delete ptr;
		al_set_event_source_data(m_ev, 0);
		return true;
	}

	bool Event_custom::emit(const char* val, const int id, std::function<void(void)> trigger_on_del)
	{
		return emit(std::string(val), id, trigger_on_del);
	}

	bool Event_custom::emit(const std::string& val, const int id, std::function<void(void)> trigger_on_del)
	{
		auto _ptr = std::unique_ptr<std::string>(new std::string(val.begin(), val.end()));
		if (emit((void*)_ptr.get(), [](void* p) {std::default_delete<std::string>()((std::string*)p); }, id, trigger_on_del)) {
			_ptr.release();
			return true;
		}
		return false;
	}

	bool Event_custom::emit(const std::vector<char>& val, const int id, std::function<void(void)> trigger_on_del)
	{
		auto _ptr = std::unique_ptr<std::vector<char>>(new std::vector<char>(val.begin(), val.end()));
		if (emit((void*)_ptr.get(), [](void* p) {std::default_delete<std::vector<char>>()((std::vector<char>*)p); }, id, trigger_on_del)) {
			_ptr.release();
			return true;
		}
		return false;
	}

	bool Event_custom::emit(std::any val, const int id, std::function<void(void)> trigger_on_del)
	{
		auto _ptr = std::unique_ptr<std::any>(new std::any(std::move(val)));
		if (emit((void*)_ptr.get(), [](void* p) {std::default_delete<std::any>()((std::any*)p); }, id, trigger_on_del)) {
			_ptr.release();
			return true;
		}
		return false;
	}

	bool Event_custom::emit(void* data, std::function<void(void*)> dtor, const int id, std::function<void(void)> trigger_on_del)
	{
		if (!m_ev) return false;
		if (!ALLEGRO_EVENT_TYPE_IS_USER(id)) throw std::invalid_argument("ID must be a USER_TYPE type to work (must follow macro ALLEGRO_EVENT_TYPE_IS_USER(X))");

		std::function<void(void*)>* _f = new std::function<void(void*)>(dtor);
		std::function<void(void)>* _tck = new std::function<void(void)>(trigger_on_del);

		ALLEGRO_EVENT ev{};
		ev.user.data1 = (intptr_t)data;
		ev.user.data2 = (intptr_t)_f;
		ev.user.data3 = (intptr_t)_tck;
		ev.user.data4 = (intptr_t)&m_accumulated_events;
		ev.user.type = id;

		++m_accumulated_events;
		return al_emit_user_event(m_ev, &ev, &__event_custom_dtor);
	}

	bool Event_custom::emit(ALLEGRO_EVENT ev, void (*dtor)(ALLEGRO_USER_EVENT*))
	{
		if (!m_ev) return false;
		if (dtor == &__event_custom_dtor) throw std::invalid_argument("This dtor is meant to be used internally only!");
		return al_emit_user_event(m_ev, &ev, dtor);
	}

	size_t Event_custom::total_on_queue() const
	{
		return m_accumulated_events;
	}

	void __event_custom_dtor(ALLEGRO_USER_EVENT* ptr)
	{
		if (!ptr) throw std::invalid_argument("Dtor failed!");
		if (ptr->data1 == 0 || ptr->data2 == 0 || ptr->data3 == 0 || ptr->data4 == 0) throw std::invalid_argument("Malformed custom event! Memory leak IS POSSIBLY A PROBLEM.");
		
		void* data_to_del = (void*)ptr->data1;

		auto destr = std::unique_ptr<std::function<void(void*)>>((std::function<void(void*)>*)ptr->data2);
		auto trace = std::unique_ptr<std::function<void(void)>>((std::function<void(void)>*)ptr->data3);
		std::atomic<size_t>& accum = *(std::atomic<size_t>*)ptr->data4;

		if (data_to_del != nullptr && destr) {
			(*destr)(data_to_del);
			if (trace && (*trace)) (*trace)();
		}
		--accum;
	}

}