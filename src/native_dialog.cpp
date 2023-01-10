#include "../include/native_dialog.h"

#ifdef _WIN32
#define GMTIM(A,B) localtime_s(B,A)
#define BADGM(X) (X != 0)
#else
#define GMTIM(A,B) localtime_r(A,B)
#define BADGM(X) (!X)
#endif

#define MAKEDAY(onfail) \
tm tm;\
time_t t = time(0);\
if (BADGM(GMTIM(&t, &tm))) {\
	return onfail;\
}

namespace AllegroCPP {

	void __native_dialog_allegro_start()
	{
		if (!al_is_system_installed() && !al_init()) throw std::runtime_error("Can't start Allegro!");
		if (!al_is_native_dialog_addon_initialized() && !al_init_native_dialog_addon()) throw std::runtime_error("Can't start Native Dialog addon!");		
	}

	Filechooser::Filechooser(Filechooser&& oth)
		: m_chosen(std::exchange(oth.m_chosen, nullptr)), m_count(std::exchange(oth.m_count, 0))
	{
	}

	void Filechooser::operator=(Filechooser&& oth)
	{
		m_chosen = std::exchange(oth.m_chosen, nullptr);
		m_count = std::exchange(oth.m_count, 0);
	}

	Filechooser::Filechooser(const std::string& initial_path, const std::string& title, const std::string& patterns_mime_semi_colon, const int flags)
	{
		__native_dialog_allegro_start();
		if (!create(initial_path, title, patterns_mime_semi_colon, flags)) throw std::invalid_argument("Could not create filechooser.");
	}

	Filechooser::~Filechooser()
	{
		destroy();
	}

	bool Filechooser::create(const std::string& initial_path, const std::string& title, const std::string& patterns_mime_semi_colon, const int flags)
	{
		destroy();
		return (m_chosen = al_create_native_file_dialog(initial_path.c_str(), title.c_str(), patterns_mime_semi_colon.c_str(), flags));
	}

	bool Filechooser::display(ALLEGRO_DISPLAY* disp)
	{
		if (m_chosen) {
			if (!al_show_native_file_dialog(disp, m_chosen)) return false;
			m_count = al_get_native_file_dialog_count(m_chosen);
		}
		return false;
	}

	void Filechooser::destroy()
	{
		if (m_chosen) {
			al_destroy_native_file_dialog(std::exchange(m_chosen, nullptr));
			m_count = 0;
		}
	}

	int Filechooser::get_count() const
	{
		return m_count;
	}

	std::string Filechooser::get_path(const size_t idx) const
	{
		if (static_cast<size_t>(m_count < 0 ? 0 : m_count) <= idx) return {};
		return al_get_native_file_dialog_path(m_chosen, idx);
	}

	std::vector<std::string> Filechooser::get_all_paths() const
	{
		std::vector<std::string> opts;

		for (size_t p = 0; p < static_cast<size_t>(m_count < 0 ? 0 : m_count); ++p) {
			const char* opt = al_get_native_file_dialog_path(m_chosen, p);
			if (opt) opts.push_back(opt);
		}

		return opts;
	}

	int message_box(const std::string& title, const std::string& head, const std::string& text, const std::string& btns, int flags, ALLEGRO_DISPLAY* dp)
	{
		return al_show_native_message_box(dp, title.c_str(), head.c_str(), text.c_str(), btns.c_str(), flags);
	}

	Text_log::ctl::ctl(const std::string& title, const bool showtim, const int flags)
		: m_show_time(showtim)
	{
		if (!(m_log = al_open_native_text_log(title.c_str(), flags))) throw std::runtime_error("Could not open native text log");
	}

	Text_log::ctl::~ctl()
	{
		if (m_buf.size()) al_append_native_text_log(m_log, "%s\n", m_buf.c_str());
		al_close_native_text_log(std::exchange(m_log, nullptr));
	}

	void Text_log::ctl::__flush()
	{
		if (m_show_time) {
			MAKEDAY();

			char _tmp[96]{};

#ifdef _WIN32
			const bool err = asctime_s(_tmp, &tm) != 0;
#else
			const bool err = asctime_r(_tmp, &tm) != nullptr;
#endif

			for (size_t p = std::size(_tmp) - 1; p > 0; --p) {
				if (_tmp[p] == '\n') {
					_tmp[p] = '\0';
					break;
				}
			}

			if (!err) m_buf = "[" + std::string(_tmp) + "] " + m_buf;
			else m_buf = "[<unknown date>] " + m_buf;
		}

		al_append_native_text_log(m_log, "%s\n", m_buf.c_str());
		m_buf.clear();
	}

	void Text_log::ctl::_write(const char* buf, size_t len)
	{
		const char* ff = buf;
		std::lock_guard<std::mutex> l(m_mtx);
		
		while(len) {
			const char* nff = (char*)memchr((void*)ff, '\n', len);
			
			if (nff == nullptr) {
				m_buf.append(ff);
				len = 0;
			}
			else {
				if (const size_t val = (nff - ff) + 1; val <= len) len -= val; // \n = +1
				else len = 0;

				m_buf.append(ff, nff - ff);
				ff = nff + 1;
				__flush();
			}
		}
	}

	Text_log::Text_log(const std::string& title, const bool showtim, const int flags)
	{
		__native_dialog_allegro_start();
		m_ctl = std::make_shared<Text_log::ctl>(title, showtim, flags);
	}

	Text_log::Text_log(const Text_log& oth)
		: m_ctl(oth.m_ctl)
	{
	}

	Text_log::Text_log(Text_log&& oth)
		: m_ctl(std::move(oth.m_ctl))
	{
	}

	void Text_log::operator=(const Text_log& oth)
	{
		m_ctl = oth.m_ctl;
	}

	void Text_log::operator=(Text_log&& oth)
	{
		m_ctl = std::move(oth.m_ctl);
	}

	bool Text_log::write(const char* dat, size_t len)
	{
		if (!m_ctl) return false;
		m_ctl->_write(dat, len);
		return true;
	}

	Text_log& Text_log::operator<<(const char* val)
	{
		this->write(val, strlen(val));
		return *this;
	}

	Text_log& Text_log::operator<<(const std::string& val)
	{
		this->write(val.data(), val.size());
		return *this;
	}

	Text_log& Text_log::operator<<(const std::vector<char>& val)
	{
		this->write(val.data(), val.size());
		return *this;
	}

	Text_log& Text_log::operator<<(bool val)
	{
		const std::string str = (val ? "1" : "0");
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(short val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(unsigned short val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(int val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(unsigned int val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(long val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(unsigned long val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(long long val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(unsigned long long val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(float val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(double val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(long double val)
	{
		std::stringstream ss(val);
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(std::streambuf* sb)
	{
		std::stringstream ss;
		ss << sb;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(std::ostream& (*pf)(std::ostream&))
	{
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(std::ios& (*pf)(std::ios&))
	{
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	Text_log& Text_log::operator<<(std::ios_base& (*pf)(std::ios_base&))
	{
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}
}