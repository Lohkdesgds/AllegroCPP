#pragma once

#include <allegro5/allegro.h>

#include <stdexcept>
#include <functional>

namespace AllegroCPP {

	class Timeout {
		ALLEGRO_TIMEOUT m_to;
	public:
		Timeout(const double);

		operator const ALLEGRO_TIMEOUT*() const;
	};

	class Mutex {
	public:
		enum class Kind { DEFAULT, RECURSIVE };
	private:
		ALLEGRO_MUTEX* m_mtx = nullptr;
		volatile bool m_was_locked = false;
	public:
		Mutex(const Mutex&) = delete;
		Mutex(Mutex&&);
		void operator=(const Mutex&) = delete;
		void operator=(Mutex&&);

		Mutex(const Kind = Kind::DEFAULT);
		~Mutex();

		void lock();
		void unlock();

		bool is_locked() const;

		operator ALLEGRO_MUTEX* ();
	};

	class Conditional {
		ALLEGRO_COND* m_cnd = nullptr;
	public:
		Conditional(const Conditional&) = delete;
		Conditional(Conditional&&);
		void operator=(const Conditional&) = delete;
		void operator=(Conditional&&) noexcept;

		Conditional();
		~Conditional();

		void wait(Mutex&);
		bool wait_until(Mutex&, const double);

		void signal_one();
		void signal_all();
		void broadcast(); // alias for signal_all
	};

	class Thread {
	public:
		enum class Mode { NORMAL_DELAYEDLAUNCH, NORMAL, DETACHED };
	private:
		struct _self {
			ALLEGRO_THREAD* m_thr = nullptr;
			volatile bool m_not_running = false; // useful
			std::function<bool(void)> m_task; // return true to call again. While(this);
			Mode m_mode = Mode::NORMAL;
		};
		_self* m_self = nullptr;

		static void* __run(ALLEGRO_THREAD* self, void* task);
		static void* __run_detached(void* task);

	public:
		Thread(const Thread&) = delete;
		Thread(Thread&&) noexcept;
		void operator=(const Thread&) = delete;
		void operator=(Thread&&) noexcept;

		Thread() = default;
		Thread(const std::function<bool(void)>, const Mode = Mode::NORMAL);

		~Thread();

		void start();
		void stop();
		void join();

		void create(const std::function<bool(void)>, const Mode);
	};

	namespace Time {
		double get_time();
		void rest(const double);
	}
}