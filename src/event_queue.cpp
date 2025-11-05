#include "event_queue.h"

namespace AllegroCPP {

	Event::Event(Event&& oth) noexcept
		: ev(oth.ev), valid_event(oth.valid_event)
	{
		oth.ev = {};
		oth.ev.type = 0; // be 100% sure
		oth.valid_event = false;
	}

	Event::Event(const ALLEGRO_EVENT& e, const bool v)
		: ev(e), valid_event(v)
	{
	}

	Event::~Event()
	{
		if (ALLEGRO_EVENT_TYPE_IS_USER(ev.type) && valid_event) {
			al_unref_user_event(&ev.user);
		}
	}

	Event::operator bool() const
	{
		return valid_event;
	}

	void Event::operator=(Event&& oth) noexcept
	{
		if (ALLEGRO_EVENT_TYPE_IS_USER(ev.type) && valid_event) {
			al_unref_user_event(&ev.user);
		}
		ev = oth.ev;
		oth.ev = {};
		oth.ev.type = 0;
		valid_event = oth.valid_event;
		oth.valid_event = false;
	}

	bool Event::empty() const
	{
		return !valid_event;
	}

	bool Event::valid() const
	{
		return valid_event;
	}

	ALLEGRO_EVENT Event::get() const
	{
		return ev;
	}

	const std::any* Event::get_data()
	{
		if (!valid_event) return nullptr;
		return (std::any*)al_get_event_source_data(ev.any.source);
	}

	bool Event::replace_data(std::any val)
	{
		if (!valid_event) return false;
		clear_data();
		al_set_event_source_data(ev.any.source, (intptr_t)(new std::any(std::move(val))));
		return true;
	}

	bool Event::clear_data()
	{
		if (!valid_event) return false;
		std::any* curr = (std::any*)al_get_event_source_data(ev.any.source);
		if (curr) delete curr;
		return true;
	}

	Event::operator ALLEGRO_EVENT() const
	{
		return ev;
	}

	Event_queue::Event_queue()
	{
		if (!al_is_system_installed()) al_init();

		if (!(m_evq = al_create_event_queue())) throw std::bad_alloc();
	}

	Event_queue::~Event_queue()
	{
		if (m_evq) al_destroy_event_queue(m_evq);
		m_evq = nullptr;
	}

	Event_queue::Event_queue(Event_queue&& oth) noexcept
		: m_evq(oth.m_evq)
	{
		oth.m_evq = nullptr;
	}

	void Event_queue::operator=(Event_queue&& oth) noexcept
	{
		if (m_evq) al_destroy_event_queue(m_evq);
		m_evq = oth.m_evq;
		oth.m_evq = nullptr;
	}

	bool Event_queue::valid() const
	{
		return m_evq != nullptr;
	}
	
	Event_queue::operator bool() const
	{
		return m_evq && !al_is_event_queue_empty(m_evq);
	}

	bool Event_queue::register_source(ALLEGRO_EVENT_SOURCE* src)
	{
		if (!src || !m_evq) return false;
		if (!al_is_event_source_registered(m_evq, src)) al_register_event_source(m_evq, src);
		return true;
	}

	Event_queue& Event_queue::operator<<(ALLEGRO_EVENT_SOURCE* src) 
	{
		register_source(src);
		return *this;
	}

	bool Event_queue::remove_source(ALLEGRO_EVENT_SOURCE* src)
	{
		if (!src || !m_evq) return false;
		if (al_is_event_source_registered(m_evq, src)) al_unregister_event_source(m_evq, src);
		return true;
	}

	void Event_queue::pause(const bool pause_it)
	{
		if (!m_evq) return;
		al_pause_event_queue(m_evq, pause_it);
	}

	bool Event_queue::is_paused() const
	{
		return m_evq ? al_is_event_queue_paused(m_evq) : false;
	}

	bool Event_queue::is_queue_empty() const
	{
		return m_evq ? al_is_event_queue_empty(m_evq) : true;
	}

	bool Event_queue::has_event() const
	{
		return m_evq ? !al_is_event_queue_empty(m_evq) : false;
	}

	Event Event_queue::get_next_event()
	{
		ALLEGRO_EVENT ev{};
		if (!m_evq) return Event{ ev, false };
		if (al_get_next_event(m_evq, &ev)) return Event{ ev, true };
		return Event{ ev, false };
	}

	Event Event_queue::peek_next_event()
	{
		ALLEGRO_EVENT ev{};
		if (!m_evq) return Event{ ev, false };
		if (al_peek_next_event(m_evq, &ev)) return Event{ ev, true };
		return Event{ ev, false };
	}

	bool Event_queue::drop_next_event()
	{
		if (!m_evq) return false;
		return al_drop_next_event(m_evq);
	}

	void Event_queue::flush_event_queue()
	{
		if (!m_evq) return;
		al_flush_event_queue(m_evq);
	}

	Event Event_queue::wait_for_event(float secs)
	{
		ALLEGRO_EVENT ev{};
		if (!m_evq) return Event{ ev, false };
		bool good = secs > 0.0f ? al_wait_for_event_timed(m_evq, &ev, secs) : true; // test directly
		if (secs <= 0.0f) al_wait_for_event(m_evq, &ev); // else wait and true.
		return Event{ ev, good };
	}

	Event Event_queue::wait_for_event(ALLEGRO_TIMEOUT* until)
	{
		ALLEGRO_EVENT ev{};
		if (!m_evq || !until) return Event{ ev, false };
		if (al_wait_for_event_until(m_evq, &ev, until)) return Event{ ev, true };
		return Event{ ev, false };
	}


}