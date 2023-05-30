#include "../include/shader.h"

namespace AllegroCPP {

	Shader::Shader(Shader&& oth) noexcept
		: m_shader(std::exchange(oth.m_shader, nullptr))
	{
	}

	void Shader::operator=(Shader&& oth) noexcept
	{
		destroy();
		m_shader = std::exchange(oth.m_shader, nullptr);
	}

	Shader::Shader(ALLEGRO_SHADER_PLATFORM platform)
	{
		if (!al_is_system_installed()) al_init();

		m_shader = al_create_shader(platform);
	}

	void Shader::destroy()
	{
		if (m_shader) {
			al_destroy_shader(m_shader);
			m_shader = nullptr;
		}
	}

	bool Shader::attach_source(const ALLEGRO_SHADER_TYPE type, const std::string& str)
	{
		if (!m_shader) return false;
		return al_attach_shader_source(m_shader, type, str.c_str());
	}

	bool Shader::detach_source(const ALLEGRO_SHADER_TYPE type)
	{
		if (!m_shader) return false;
		return al_attach_shader_source(m_shader, type, nullptr);
	}

	bool Shader::attach_source_file(const ALLEGRO_SHADER_TYPE type, const std::string& str)
	{
		if (!m_shader) return false;
		return al_attach_shader_source_file(m_shader, type, str.c_str());
	}

	bool Shader::detach_source_file(const ALLEGRO_SHADER_TYPE type)
	{
		if (!m_shader) return false;
		return al_attach_shader_source_file(m_shader, type, nullptr);
	}

	bool Shader::build()
	{
		if (!m_shader) return false;
		return al_build_shader(m_shader);
	}

	std::string Shader::get_log() const
	{
		return m_shader ? al_get_shader_log(m_shader) : "";
	}

	ALLEGRO_SHADER_PLATFORM Shader::get_platform() const
	{
		return m_shader ? al_get_shader_platform(m_shader) : ALLEGRO_SHADER_AUTO;
	}

	bool Shader::use()
	{
		return m_shader ? al_use_shader(m_shader) : false;
	}

	bool Shader::set_sampler(const std::string& name, ALLEGRO_BITMAP* video_bmp, const int unit)
	{
		return al_set_shader_sampler(name.c_str(), video_bmp, unit);
	}

	bool Shader::set_matrix(const std::string& name, const ALLEGRO_TRANSFORM* transform)
	{
		return al_set_shader_matrix(name.c_str(), transform);
	}

	bool Shader::set_int(const std::string& name, const int i)
	{
		return al_set_shader_int(name.c_str(), i);
	}

	bool Shader::set_float(const std::string& name, const float f)
	{
		return al_set_shader_float(name.c_str(), f);
	}

	bool Shader::set_bool(const std::string& name, const bool b)
	{
		return al_set_shader_bool(name.c_str(), b);
	}

	bool Shader::set_int_vector(const std::string& name, const int num_components, const int* i, const int num_elements)
	{
		return al_set_shader_int_vector(name.c_str(), num_components, i, num_elements);
	}

	bool Shader::set_float_vector(const std::string& name, const int num_components, const float* f, const int num_elements)
	{
		return al_set_shader_float_vector(name.c_str(), num_components, f, num_elements);
	}

	std::string Shader::default_source(const ALLEGRO_SHADER_PLATFORM platform, const ALLEGRO_SHADER_TYPE type)
	{
		return al_get_default_shader_source(platform, type);
	}

}