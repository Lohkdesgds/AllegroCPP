#include "../include/thread.h"

namespace AllegroCPP {

	Timeout::Timeout(const double timeout_sec)
	{
		if (al_is_system_installed()) al_init();
		al_init_timeout(&m_to, timeout_sec);
	}

	Timeout::operator const ALLEGRO_TIMEOUT* () const
	{
		return &m_to;
	}

	Mutex::Mutex(Mutex&& oth)
		: m_mtx(oth.m_mtx), m_was_locked(oth.m_was_locked)
	{
		if (!oth.m_mtx) throw std::invalid_argument("Moved invalid mutex!");
		al_lock_mutex(m_mtx);
		oth.m_mtx = nullptr;
		oth.m_was_locked = false;
		al_unlock_mutex(m_mtx);
	}

	void Mutex::operator=(Mutex&& oth)
	{
		if (m_was_locked) throw std::runtime_error("Mutex being overwritten was locked, can't move!");
		
		al_lock_mutex(oth.m_mtx);

		al_destroy_mutex(m_mtx);
		m_mtx = std::exchange(oth.m_mtx, nullptr);
		m_was_locked = std::exchange(oth.m_was_locked, false);
		al_unlock_mutex(oth.m_mtx);
	}

	Mutex::Mutex(const Kind kind)
	{
		if (!al_is_system_installed()) al_init();
		switch (kind) {
		case Kind::DEFAULT:
			m_mtx = al_create_mutex();
			break;
		case Kind::RECURSIVE:
			m_mtx = al_create_mutex_recursive();
			break;
		}
	}

	Mutex::~Mutex()
	{
		if (m_mtx) {
			al_destroy_mutex(m_mtx);
			m_was_locked = false;
			m_mtx = nullptr;
		}
	}

	void Mutex::lock()
	{
		al_lock_mutex(m_mtx);
		m_was_locked = true;
	}

	void Mutex::unlock()
	{
		m_was_locked = false;
		al_unlock_mutex(m_mtx);
	}

	bool Mutex::is_locked() const
	{
		return m_was_locked;
	}

	Mutex::operator ALLEGRO_MUTEX* ()
	{
		return m_mtx;
	}


	Conditional::Conditional(Conditional&& oth)
		: m_cnd(oth.m_cnd)
	{
		if (!oth.m_cnd) throw std::invalid_argument("Moved invalid cond!");
		oth.m_cnd = nullptr;
	}

	void Conditional::operator=(Conditional&& oth) noexcept
	{
		al_destroy_cond(m_cnd);
		m_cnd = std::exchange(oth.m_cnd, nullptr);
	}

	Conditional::Conditional()
	{
		if (!al_is_system_installed()) al_init();
		m_cnd = al_create_cond();
	}

	Conditional::~Conditional()
	{
		if (m_cnd) {
			al_destroy_cond(m_cnd);
			m_cnd = nullptr;
		}
	}

	void Conditional::wait(Mutex& mtx)
	{
		al_wait_cond(m_cnd, mtx);
	}

	bool Conditional::wait_until(Mutex& mtx, double time_sec)
	{
		Timeout to(time_sec);
		return al_wait_cond_until(m_cnd, mtx, to) == 0; // 0 == success...
	}

	void Conditional::signal_one()
	{
		// Note: The pthreads spec says to lock the mutex associated with cond before signalling for predictable scheduling behaviour.
		// source: Allegro
		al_signal_cond(m_cnd);
	}

	void Conditional::signal_all()
	{
		// Note: The pthreads spec says to lock the mutex associated with cond before signalling for predictable scheduling behaviour.
		// source: Allegro
		al_broadcast_cond(m_cnd);
	}

	void Conditional::broadcast()
	{
		// Note: The pthreads spec says to lock the mutex associated with cond before signalling for predictable scheduling behaviour.
		// source: Allegro
		al_broadcast_cond(m_cnd);
	}

	void* Thread::__run(ALLEGRO_THREAD* self, void* ptr)
	{
		Thread::_self* tasker = (Thread::_self*)ptr;
		tasker->m_not_running = false;
		while (!al_get_thread_should_stop(self)) {
			if (!tasker->m_task()) break;
		}

		tasker->m_not_running = true;
		return nullptr;
	}

	void* Thread::__run_detached(void* ptr)
	{
		Thread::_self* tasker = (Thread::_self*)ptr;

		struct _autodel {
			Thread::_self* _hold{};
			_autodel(Thread::_self* s) { _hold = s; }
			~_autodel() { delete _hold; } // kills if dead
		} _auto(tasker);

		tasker->m_not_running = false;

		while (!tasker->m_not_running) {
			if (!tasker->m_task()) break;
		}

		return nullptr;
	}
	
	Thread::Thread(Thread&& oth) noexcept
		: m_self(std::exchange(oth.m_self, nullptr))
	{		
	}

	void Thread::operator=(Thread&& oth) noexcept
	{
		join();
		m_self = std::exchange(oth.m_self, nullptr);
	}
	
	Thread::Thread(const std::function<bool(void)> f, const Mode mode)
	{
		create(f, mode);
	}

	Thread::~Thread()
	{
		join();
	}

	void Thread::start()
	{
		if (m_self) {
			switch (m_self->m_mode) {
			case Mode::NORMAL_DELAYEDLAUNCH:
				if (m_self->m_not_running) {
					al_start_thread(m_self->m_thr);
					m_self->m_not_running = false; // just to be sure, idk, what if you call stop()...
				}
				break;
			default: // others just launch on creation
				break;
			}
		}
	}

	void Thread::stop()
	{
		if (m_self) {
			switch (m_self->m_mode) {
			case Mode::NORMAL:
			case Mode::NORMAL_DELAYEDLAUNCH:
				if (!m_self->m_not_running) al_set_thread_should_stop(m_self->m_thr);
				break;
			case Mode::DETACHED:
				m_self->m_not_running = true;
				m_self = nullptr; // consider stopped. It kills itself sometime soon (if you don't mess up).
				break;
			}
		}
	}

	void Thread::join()
	{
		stop();
		if (m_self) {
			switch (m_self->m_mode) {
			case Mode::NORMAL:
			case Mode::NORMAL_DELAYEDLAUNCH:
				if (!m_self->m_not_running) al_join_thread(m_self->m_thr, nullptr);
				al_destroy_thread(m_self->m_thr);
				delete m_self;
				m_self = nullptr;
				break;
			case Mode::DETACHED:
				m_self->m_not_running = true; // detached thread must kill the resources itself.
				m_self = nullptr; // consider stopped. It kills itself sometime soon (if you don't mess up).
				break;
			}
		}
	}

	void Thread::create(const std::function<bool(void)> f, const Mode mode)
	{
		join();

		if (!f) throw std::invalid_argument("No function set in thread creation, nothing to do, what will I create then?");
		if (!al_is_system_installed()) al_init();

		m_self = new Thread::_self();
		m_self->m_mode = mode;
		m_self->m_task = f;

		switch (m_self->m_mode) {
		case Mode::NORMAL_DELAYEDLAUNCH:
			m_self->m_thr = al_create_thread(__run, (void*)m_self);
			break;
		case Mode::NORMAL:
			m_self->m_thr = al_create_thread(__run, (void*)m_self);
			al_start_thread(m_self->m_thr);
			break;
		case Mode::DETACHED:
			m_self->m_not_running = true; // using this as start flag

			al_run_detached_thread(__run_detached, (void*)m_self);

			for (size_t _w = 0; m_self->m_not_running && _w < 50; ++_w) Time::rest(0.02); // wait for wake up. This makes sure there's no memory leak.

			if (m_self->m_not_running) {
				delete m_self; // may crash, but if it took > 1 second to launch, it is not launching anytime soon.
				m_self = nullptr;
				throw std::runtime_error("Could not launch thread in time! Detached thread launch failed");
			}
			break;
		}
	}

	namespace Time {

		double get_time()
		{
			return al_get_time();
		}

		void rest(const double seconds)
		{
			al_rest(seconds);
		}
	}
}