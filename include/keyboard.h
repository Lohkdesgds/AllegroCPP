#pragma once

#include <allegro5/allegro.h>

#include <string>
#include <stdexcept>

namespace AllegroCPP {

	class Keyboard {
		ALLEGRO_KEYBOARD_STATE m_state{};
	public:
		Keyboard();

		bool get_key_down(const int) const;

		static std::string keycode_to_str(const int);
		static bool set_leds(const int);

		ALLEGRO_KEYBOARD_STATE get() const;
	};

}