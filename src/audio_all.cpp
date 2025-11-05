#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdexcept>
#include <utility>

namespace AllegroCPP {

	// if samples < 0 -> reserve only if not started. if samples > 0 -> call reserve no matter what. if samples == 0 -> default
	void __audio_allegro_start(int samples)
	{
		static bool has_sampled = false;

		if (samples == 0) samples = -16; // -16 -> start with 16 if never started

		if (!al_is_system_installed() && !al_init()) throw std::runtime_error("Can't start Allegro!");
		if ((!al_is_audio_installed() && !al_install_audio()) || (!al_is_acodec_addon_initialized() && !al_init_acodec_addon()) || (std::exchange(has_sampled, true) ? false : (!al_reserve_samples(samples < 0 ? -samples : samples)))) {
			throw std::runtime_error("Can't start Audio or Audio Codec!");
		}
		else if (samples > 0) al_reserve_samples(samples); // maybe it allows re-reserving more. 		
	}

}