#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <mutex>

namespace AllegroCPP {

	class Filechooser {
		ALLEGRO_FILECHOOSER* m_chosen = nullptr;
		int m_count = 0;
	public:
		Filechooser(const Filechooser&) = delete;
		void operator=(const Filechooser&) = delete;

		Filechooser(Filechooser&&);
		void operator=(Filechooser&&);
		Filechooser(const std::string&, const std::string&, const std::string&, const int = 0);
		~Filechooser();
		
		bool create(const std::string&, const std::string&, const std::string&, const int = 0);

		bool display(ALLEGRO_DISPLAY* = nullptr);
		void destroy();

		int get_count() const;
		std::string get_path(const size_t) const;

		std::vector<std::string> get_all_paths() const;
	};

	class Text_log {
		struct ctl {
			ALLEGRO_TEXTLOG* m_log = nullptr;
			std::string m_buf;
			std::mutex m_mtx;
			const bool m_show_time;

			ctl(const std::string&, const bool = true, const int = 0);
			~ctl();

			void __flush();
			void _write(const char*, size_t);
		};
		std::shared_ptr<ctl> m_ctl;
	public:
		Text_log(const std::string&, const bool = true, const int = 0);
		Text_log(const Text_log&);
		Text_log(Text_log&&);
		void operator=(const Text_log&);
		void operator=(Text_log&&);

		bool write(const char*, size_t);
		Text_log& operator<<(const char* val);
		Text_log& operator<<(const std::string& val);
		Text_log& operator<<(const std::vector<char>& val);
		Text_log& operator<<(bool val);
		Text_log& operator<<(short val);
		Text_log& operator<<(unsigned short val);
		Text_log& operator<<(int val);
		Text_log& operator<<(unsigned int val);
		Text_log& operator<<(long val);
		Text_log& operator<<(unsigned long val);
		Text_log& operator<<(long long val);
		Text_log& operator<<(unsigned long long val);
		Text_log& operator<<(float val);
		Text_log& operator<<(double val);
		Text_log& operator<<(long double val);
		Text_log& operator<<(std::streambuf* sb);
		Text_log& operator<<(std::ostream& (*pf)(std::ostream&));
		Text_log& operator<<(std::ios& (*pf)(std::ios&));
		Text_log& operator<<(std::ios_base& (*pf)(std::ios_base&));
	};

	/// <summary>
	/// Generate a pop up in messagebox style
	/// </summary>
	/// <param name="{std::string}">Title</param>
	/// <param name="{std::string}">Header</param>
	/// <param name="{std::string}">Body text</param>
	/// <param name="{std::string}">Buttons (doesn't work on Windows)</param>
	/// <param name="{int}">Flags</param>
	/// <param name="{ALLEGRO_DISPLAY*}">Referenced as parent display if possible, optional</param>
	/// <returns>{int} 0 if closed or failed, 1 for OK or Yes, 2 for Cancel or No. If custom buttons, they start at 1.</returns>
	int message_box(const std::string&, const std::string&, const std::string&, const std::string& = "", int = ALLEGRO_MESSAGEBOX_WARN, ALLEGRO_DISPLAY* = nullptr);

}