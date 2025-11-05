#include "path.h"

#include <utility>

namespace AllegroCPP {

	Path::Path(const Path& oth)
		: m_path(oth.m_path ? al_clone_path(oth.m_path) : nullptr)
	{		
	}

	Path& Path::operator=(const Path& oth)
	{
		destroy();
		m_path = oth.m_path ? al_clone_path(oth.m_path) : nullptr;
		return *this;
	}

	Path::Path(Path&& oth) noexcept
		: m_path(std::exchange(oth.m_path, nullptr))
	{
	}

	void Path::operator=(Path&& oth) noexcept
	{
		destroy();
		m_path = std::exchange(oth.m_path, nullptr);
	}

	Path::Path(const std::string& path, const bool force_as_directory)
		: m_path(force_as_directory ? al_create_path_for_directory(path.length() ? path.c_str() : nullptr) : al_create_path(path.length() ? path.c_str() : nullptr))
	{
	}

	Path::~Path()
	{
		destroy();
	}

	void Path::create(const std::string& path, const bool force_as_directory)
	{
		destroy();
		m_path = (force_as_directory ? al_create_path_for_directory(path.length() ? path.c_str() : nullptr) : al_create_path(path.length() ? path.c_str() : nullptr));
	}

	void Path::clone(const Path& oth)
	{
		destroy();
		m_path = oth.m_path ? al_clone_path(oth.m_path) : nullptr;
	}

	void Path::destroy()
	{
		if (m_path) {
			al_destroy_path(m_path);
			m_path = nullptr;
		}
	}

	bool Path::join(const Path& oth)
	{
		return m_path ? (oth.m_path ? al_join_paths(m_path, oth.m_path) : true) : false;
	}

	bool Path::join(const std::string& str)
	{
		Path oth(str, true);
		return join(oth);
	}

	bool Path::rebase(Path&& oth) const
	{
		return m_path ? (oth.m_path ? al_rebase_path(m_path, oth.m_path) : false) : true;
	}

	std::string Path::get_drive() const
	{
		return m_path ? al_get_path_drive(m_path) : "";
	}

	int Path::get_num_components() const
	{
		return m_path ? al_get_path_num_components(m_path) : 0;
	}

	std::string Path::get_component(const int index) const
	{
		return m_path ? al_get_path_component(m_path, index) : "";
	}

	std::string Path::get_tail() const
	{
		return m_path ? al_get_path_tail(m_path) : "";
	}

	std::string Path::get_filename() const
	{
		return m_path ? al_get_path_filename(m_path) : "";
	}

	std::string Path::get_basename() const
	{
		return m_path ? al_get_path_basename(m_path) : "";
	}

	std::string Path::get_extension() const
	{
		return m_path ? al_get_path_extension(m_path) : "";
	}

	void Path::set_drive(const std::string& str) const
	{
		if (m_path) al_set_path_drive(m_path, str.length() ? str.c_str() : nullptr);
	}

	void Path::append(const std::string& str)
	{
		if (m_path && str.length()) al_append_path_component(m_path, str.c_str());
	}

	void Path::insert(const int pos, const std::string& str)
	{
		if (m_path && str.length()) al_insert_path_component(m_path, pos, str.c_str());
	}

	void Path::replace(const int pos, const std::string& str)
	{
		if (m_path && str.length()) al_replace_path_component(m_path, pos, str.c_str());
	}

	void Path::remove(const int pos)
	{
		if (m_path) al_remove_path_component(m_path, pos);
	}

	void Path::drop_tail()
	{
		if (m_path) al_drop_path_tail(m_path);
	}

	void Path::pop_back()
	{
		if (m_path) al_drop_path_tail(m_path);
	}

	void Path::set_filename(const std::string& str)
	{
		if (m_path) al_set_path_filename(m_path, str.length() ? str.c_str() : nullptr);
	}

	void Path::set_extension(const std::string& str)
	{
		if (m_path) al_set_path_extension(m_path, str.length() ? str.c_str() : nullptr);
	}

	std::string Path::c_str(const char delim) const
	{
		return m_path ? al_path_cstr(m_path, delim) : "";
	}

	bool Path::make_canonical()
	{
		return m_path ? al_make_path_canonical(m_path) : false;
	}

	Path& Path::operator+=(const Path& oth)
	{
		append(oth.c_str());
		return *this;
	}

	Path Path::operator+(const Path& oth)
	{
		Path copy = *this;
		copy.append(oth.c_str());
		return copy;
	}

	Path& Path::operator+=(const std::string& str)
	{
		return this->operator+=(Path(str));
	}

	Path Path::operator+(const std::string& str)
	{
		return this->operator+(Path(str));
	}

	Path::operator ALLEGRO_PATH* () const
	{
		return m_path;
	}

}
