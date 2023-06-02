#include "../include/config.h"

namespace AllegroCPP {

	Config_iterator::Config_iterator(ALLEGRO_CONFIG* cfg, bool as_end)
		: m_cfg(cfg)
	{
		if (!cfg) throw std::invalid_argument("Invalid config");

		if (!as_end) {
			m_hold.m_currsec = al_get_first_config_section(m_cfg, &m_it);
			if (!m_hold.m_currsec) return;
			m_hold.m_currkey = al_get_first_config_entry(m_cfg, m_hold.m_currsec, &m_it_en);
			if (!m_hold.m_currkey) return;
			m_hold.m_currval = al_get_config_value(m_cfg, m_hold.m_currsec, m_hold.m_currkey);
		}
	}

	bool Config_iterator::operator==(const Config_iterator& oth) const
	{
		return 
			oth.m_hold.m_currsec == m_hold.m_currsec &&
			oth.m_hold.m_currkey == m_hold.m_currkey &&
			oth.m_hold.m_currval == m_hold.m_currval;
	}

	bool Config_iterator::operator!=(const Config_iterator& oth) const
	{
		return
			oth.m_hold.m_currsec != m_hold.m_currsec ||
			oth.m_hold.m_currkey != m_hold.m_currkey ||
			oth.m_hold.m_currval != m_hold.m_currval;
	}

	Config_iterator& Config_iterator::operator++()
	{
		if (!m_hold.m_currsec) return *this;
		m_hold.m_currkey = al_get_next_config_entry(&m_it_en);
		if (!m_hold.m_currkey) {
			m_hold.m_currsec = al_get_next_config_section(&m_it);
			if (!m_hold.m_currsec) {
				m_hold.m_currval = nullptr;
				return *this;
			}
			m_hold.m_currkey = al_get_first_config_entry(m_cfg, m_hold.m_currsec, &m_it_en);
			if (!m_hold.m_currkey) m_hold.m_currval = nullptr;
			else m_hold.m_currval = al_get_config_value(m_cfg, m_hold.m_currsec, m_hold.m_currkey);
			return *this;
		}
		m_hold.m_currval = al_get_config_value(m_cfg, m_hold.m_currsec, m_hold.m_currkey);
		return *this;
	}

	Config_iterator Config_iterator::operator++(int)
	{
		Config_iterator act = *this;
		++(*this);
		return act;
	}

	const Config_iterator::hold& Config_iterator::operator*() const
	{
		return m_hold;
	}

	std::string Config_iterator::get_section() const
	{
		return m_hold.m_currsec ? std::string(m_hold.m_currsec) : std::string{};
	}

	std::string Config_iterator::get_key() const
	{
		return m_hold.m_currkey ? std::string(m_hold.m_currkey) : std::string{};
	}

	std::string Config_iterator::get_value() const
	{
		return m_hold.m_currval ? std::string(m_hold.m_currval) : std::string{};
	}

	Config::Config()
	{
		if (!al_is_system_installed()) al_init();
		m_conf = al_create_config();
		if (!m_conf) throw std::bad_alloc();
	}

	Config::Config(const std::string& path)
	{
		if (path.empty()) throw std::invalid_argument("Config path was nullptr");
		if (!al_is_system_installed()) al_init();
		m_conf = al_load_config_file(path.c_str());
		if (!m_conf) throw std::bad_alloc();
	}

	Config::Config(ALLEGRO_FILE* fp)
	{
		if (!fp) throw std::invalid_argument("Config was nullptr");
		if (!al_is_system_installed()) al_init();
		m_conf = al_load_config_file_f(fp);
		if (!m_conf) throw std::bad_alloc();
	}

	Config::~Config()
	{
		if (m_conf) al_destroy_config(m_conf);
		m_conf = nullptr;
	}

	Config::Config(Config&& oth) noexcept
		: m_conf(oth.m_conf)
	{
		oth.m_conf = nullptr;
	}

	void Config::operator=(Config&& oth) noexcept
	{
		if (m_conf) al_destroy_config(m_conf);
		m_conf = oth.m_conf;
		oth.m_conf = nullptr;
	}

	Config_iterator Config::begin() const
	{
		return Config_iterator{ m_conf, false };
	}

	Config_iterator Config::end() const
	{
		return Config_iterator{ m_conf, true };
	}

	bool Config::valid() const
	{
		return m_conf != nullptr;
	}

	bool Config::empty() const
	{
		return m_conf == nullptr;
	}

	Config::operator bool() const
	{
		return m_conf != nullptr;
	}

	bool Config::save(const std::string& path) const
	{
		if (!m_conf) return false;
		return al_save_config_file(path.c_str(), m_conf);
	}

	bool Config::save(ALLEGRO_FILE* fp) const
	{
		if (!m_conf) return false;
		return al_save_config_file_f(fp, m_conf);
	}

	bool Config::merge(Config&& oth)
	{
		if (!m_conf || !oth.m_conf) return false;
		al_merge_config_into(m_conf, oth.m_conf);
		return true;
	}

	bool Config::add_section(const std::string& section)
	{
		if (!m_conf || section.empty()) return false;
		al_add_config_section(m_conf, section.c_str());
		return true;
	}

	bool Config::remove_section(const std::string& section)
	{
		if (!m_conf || section.empty()) return false;
		return al_remove_config_section(m_conf, section.c_str());
	}

	bool Config::comment(const std::string& section, const std::string& comment)
	{
		if (!m_conf || section.empty()) return false;
		al_add_config_comment(m_conf, section.c_str(), comment.c_str());
		return true;
	}

	std::string Config::get(const std::string& section, const std::string& key) const
	{
		if (!m_conf || section.empty() || key.empty()) return {};
		const char* str = al_get_config_value(m_conf, section.c_str(), key.c_str());
		return str ? str : "";
	}

	bool Config::set(const std::string& section, const std::string& key, const std::string& value)
	{
		if (!m_conf || section.empty() || key.empty()) return false;
		al_set_config_value(m_conf, section.c_str(), key.c_str(), value.c_str());
		return true;
	}

	bool Config::remove(const std::string& section, const std::string& key)
	{
		if (!m_conf || section.empty() || key.empty()) return false;
		return al_remove_config_key(m_conf, section.c_str(), key.c_str());
	}

	Config_system::Config_system()
	{
		if (!al_is_system_installed()) al_init();
		m_conf = al_get_system_config();
	}

	Config_system::~Config_system()
	{
		m_conf = nullptr; // before ~Config()
	}

}