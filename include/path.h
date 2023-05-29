#pragma once

#include <allegro5/allegro.h>

#include <string>

namespace AllegroCPP {

	class Path {
		ALLEGRO_PATH* m_path = nullptr;
	public:
		Path(const Path&);
		Path& operator=(const Path&);
		Path(Path&&) noexcept;
		void operator=(Path&&) noexcept;

		Path(const std::string& path = {}, const bool force_as_directory = false);
		~Path();

		void create(const std::string& path = {}, const bool force_as_directory = false);
		void clone(const Path&);
		void destroy();

		// only works if argument is relative path
		bool join(const Path&);
		// only works if argument is relative path
		bool join(const std::string&);

		// Rebase argument with this as reference. This is inserted at the beginning of argument.
		bool rebase(Path&&) const;

		std::string get_drive() const;
		// Get number of directory components in this path (depth)
		int get_num_components() const;

		std::string get_component(const int) const;
		std::string get_tail() const;
		std::string get_filename() const;
		std::string get_basename() const;
		std::string get_extension() const;
		
		void set_drive(const std::string&) const;

		void append(const std::string&);
		void insert(int, const std::string&);
		void replace(int, const std::string&);
		void remove(int);

		// remove last directory component, if any
		void drop_tail();
		// same as drop tail
		void pop_back();

		void set_filename(const std::string&);
		void set_extension(const std::string&);

		std::string c_str(const char delim = ALLEGRO_NATIVE_PATH_SEP) const;

		bool make_canonical();

		Path& operator+=(const Path&);
		Path operator+(const Path&);

		Path& operator+=(const std::string&);
		Path operator+(const std::string&);
	};

}