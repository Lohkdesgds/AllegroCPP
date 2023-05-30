#include "../include/keyboard.h"

namespace AllegroCPP {

	Keyboard::Keyboard()
	{
		if (!al_is_system_installed()) al_init();
		if (!al_is_keyboard_installed()) al_install_keyboard();
		al_get_keyboard_state(&m_state);
	}

	bool Keyboard::get_key_down(const int keycode) const
	{
		return al_key_down(&m_state, keycode);
	}

	std::string Keyboard::keycode_to_str(const int keycode)
	{
		return al_keycode_to_name(keycode);
	}

	bool Keyboard::set_leds(const int leds)
	{
		return al_set_keyboard_leds(leds);
	}

	ALLEGRO_KEYBOARD_STATE Keyboard::get() const
	{
		return m_state;
	}
}