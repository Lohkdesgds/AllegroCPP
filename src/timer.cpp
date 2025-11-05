#include "timer.h"

namespace AllegroCPP {

	Timer::Timer(Timer&& oth) noexcept
		: m_tim(std::exchange(oth.m_tim, nullptr))
	{
	}

	void Timer::operator=(Timer&& oth) noexcept
	{
		if (m_tim) al_destroy_timer(m_tim);
		m_tim = std::exchange(oth.m_tim, nullptr);
	}

	Timer::Timer(const double sec)
	{
		if (!al_is_system_installed()) al_init();
		m_tim = al_create_timer(sec);
	}

	void Timer::start()
	{
		al_start_timer(m_tim);
	}

	void Timer::stop()
	{
		al_stop_timer(m_tim);
	}

	bool Timer::started() const
	{
		return m_tim && al_get_timer_started(m_tim);
	}

	int64_t Timer::count() const
	{
		return al_get_timer_count(m_tim);
	}

	void Timer::set_count(const int64_t c)
	{
		al_set_timer_count(m_tim, c);
	}

	void Timer::add_count(const int64_t c)
	{
		al_add_timer_count(m_tim, c);
	}

	double Timer::get_speed() const
	{
		return al_get_timer_speed(m_tim);
	}

	void Timer::set_speed(const double sec)
	{
		al_set_timer_speed(m_tim, sec);
	}

	Timer::operator ALLEGRO_TIMER* ()
	{
		return m_tim;
	}

	Timer::operator ALLEGRO_EVENT_SOURCE* ()
	{
		return al_get_timer_event_source(m_tim);
	}

	Timer::operator bool() const
	{
		return m_tim;
	}


}