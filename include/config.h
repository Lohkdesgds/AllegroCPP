#pragma once

#include <allegro5/allegro.h>

#include <string>
#include <stdexcept>

namespace AllegroCPP {

	class Config_iterator {
	public:
		struct hold {
			const char* m_currsec = nullptr;
			const char* m_currkey = nullptr;
			const char* m_currval = nullptr;
		};
	private:
		ALLEGRO_CONFIG* m_cfg;

		ALLEGRO_CONFIG_SECTION* m_it = nullptr;
		ALLEGRO_CONFIG_ENTRY* m_it_en = nullptr;

		hold m_hold;
	public:
		Config_iterator(ALLEGRO_CONFIG*, bool as_end);

		bool operator==(const Config_iterator&) const;
		bool operator!=(const Config_iterator&) const;

		Config_iterator& operator++();
		Config_iterator operator++(int);

		const hold& operator*() const;

		std::string get_section() const;
		std::string get_key() const;
		std::string get_value() const;
	};

	class Config {
	protected:
		ALLEGRO_CONFIG* m_conf = nullptr;
	public:
		Config();
		Config(const std::string& path);
		Config(ALLEGRO_FILE* fp);
		virtual ~Config();

		Config(const Config&) = delete;
		Config(Config&&) noexcept;
		void operator=(const Config&) = delete;
		void operator=(Config&&) noexcept;

		Config_iterator begin() const;
		Config_iterator end() const;

		bool valid() const;
		bool empty() const;
		operator bool() const;

		bool merge(Config&&);

		bool add_section(const std::string& section);
		bool remove_section(const std::string& section);

		bool comment(const std::string& section, const std::string& comment);

		std::string get(const std::string& section, const std::string& key) const;
		bool set(const std::string& section, const std::string& key, const std::string& value);
		bool remove(const std::string& section, const std::string& key);
	};

	class Config_system : public Config {
	public:
		Config_system();
		~Config_system();

		Config_system(const Config_system&) = delete;
		Config_system(Config_system&&) = delete;
		void operator=(const Config_system&) = delete;
		void operator=(Config_system&&) = delete;
	};

}