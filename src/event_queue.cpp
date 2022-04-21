#include "../include/event_queue.h"

namespace AllegroCPP {

	Event::operator bool() const
	{
		return valid_event;
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
		return m_evq != nullptr;
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