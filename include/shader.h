#pragma once

#include <allegro5/allegro.h>

#include <string>

namespace AllegroCPP {

	// Note from Allegro:
	// If you do not use ALLEGRO_PROGRAMMABLE_PIPELINE Allegro’s graphics functions will not use any shader specific functions themselves.

	class Shader {
		ALLEGRO_SHADER* m_shader = nullptr;
	public:
		Shader(const Shader&) = delete;
		void operator=(const Shader&) = delete;

		Shader(Shader&&) noexcept;
		void operator=(Shader&&) noexcept;

		Shader(ALLEGRO_SHADER_PLATFORM = ALLEGRO_SHADER_AUTO);

		void destroy();

		bool attach_source(const ALLEGRO_SHADER_TYPE, const std::string&);
		bool detach_source(const ALLEGRO_SHADER_TYPE);
		bool attach_source_file(const ALLEGRO_SHADER_TYPE, const std::string&);
		bool detach_source_file(const ALLEGRO_SHADER_TYPE);

		bool build();

		std::string get_log() const;
		ALLEGRO_SHADER_PLATFORM get_platform() const;

		bool use();

		static bool set_sampler(const std::string&, ALLEGRO_BITMAP*, const int);
		static bool set_matrix(const std::string&, const ALLEGRO_TRANSFORM*);
		static bool set_int(const std::string&, const int);
		static bool set_float(const std::string&, const float);
		static bool set_bool(const std::string&, const bool);
		// int flowers[4][3] -> (..., 4, flowers, 3)
		static bool set_int_vector(const std::string&, const int num_components, const int* i, const int num_elements);
		static bool set_float_vector(const std::string&, const int num_components, const float* f, const int num_elements);

		static std::string default_source(const ALLEGRO_SHADER_PLATFORM platform, const ALLEGRO_SHADER_TYPE type);
	};

}