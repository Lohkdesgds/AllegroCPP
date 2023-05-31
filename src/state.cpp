#include "../include/state.h"

namespace AllegroCPP {

	State::State(State&& oth) noexcept
		: m_state(std::exchange(oth.m_state, nullptr))
	{
	}

	void State::operator=(State&& oth) noexcept
	{
		m_state = std::exchange(oth.m_state, nullptr);
	}

	State::~State()
	{
		if (m_state) {
			delete m_state;
			m_state = nullptr;
		}
	}

	void State::store(int flags)
	{
		if (m_state) delete m_state;
		m_state = new ALLEGRO_STATE{};

		al_store_state(m_state, flags);
	}

	void State::restore()
	{
		if (m_state) al_restore_state(m_state);
	}

	int get_errno()
	{
		return al_get_errno();
	}

	void set_errno(const int val)
	{
		al_set_errno(val);
	}
}