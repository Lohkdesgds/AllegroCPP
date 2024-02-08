#pragma once

#include <allegro5/allegro.h>

#include <string>

namespace AllegroCPP {

	class State {
		ALLEGRO_STATE* m_state = nullptr;
	public:
		State(const State&) = delete;
		void operator=(const State&) = delete;

		State(State&&) noexcept;
		void operator=(State&&) noexcept;

		State() = default;
		~State();

		void store(const int flags);
		void restore();
	};	

	int get_errno();
	void set_errno(const int);
}