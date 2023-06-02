#include "../include/utfstring.h"

namespace AllegroCPP {

	UTFstring::UTFstring()
	{
		if (!al_is_system_installed()) al_init();
		m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_new(""), [](ALLEGRO_USTR* s) {al_ustr_free(s); });
	}

	UTFstring::UTFstring(const std::string& str)
	{
		if (!al_is_system_installed()) al_init();
		m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_new_from_buffer(str.c_str(), str.length()), [](ALLEGRO_USTR* s) {al_ustr_free(s); });
	}

	UTFstring::UTFstring(const uint16_t* const s)
	{
		if (!al_is_system_installed()) al_init();
		m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_new_from_utf16(s), [](ALLEGRO_USTR* s) {al_ustr_free(s); });
	}

	UTFstring::UTFstring(const UTFstring& oth)
		: m_string(std::shared_ptr<ALLEGRO_USTR>(al_ustr_dup(oth.m_string.get()), [](ALLEGRO_USTR* s) {al_ustr_free(s); }))
	{		
	}

	void UTFstring::to_buffer(char* buf, size_t lim)
	{
		if (!m_string) return;

		if (lim > std::numeric_limits<int>::max()) lim = std::numeric_limits<int>::max();
		al_ustr_to_buffer(m_string.get(), buf, static_cast<int>(lim));
	}

	UTFstring UTFstring::substr_bytes(const size_t start_pos, const size_t end_pos)
	{
		UTFstring str;
		str.m_string = std::shared_ptr<ALLEGRO_USTR>(
			al_ustr_dup_substr(m_string.get(), static_cast<int>(start_pos), static_cast<int>(end_pos)), 
			[](ALLEGRO_USTR* s) {al_ustr_free(s); }
		);
		return str;
	}

	UTFstring UTFstring::substr(const size_t start_pos, const size_t end_pos)
	{
		UTFstring str;
		str.m_string = std::shared_ptr<ALLEGRO_USTR>(
			al_ustr_dup_substr(m_string.get(), al_ustr_offset(m_string.get(), static_cast<int>(start_pos)), al_ustr_offset(m_string.get(), static_cast<int>(end_pos))),
			[](ALLEGRO_USTR* s) {al_ustr_free(s); }
		);
		return str;
	}

	size_t UTFstring::size() const
	{
		return m_string ? al_ustr_size(m_string.get()) : 0;
	}

	size_t UTFstring::length() const
	{
		return m_string ? al_ustr_length(m_string.get()) : 0;
	}

	int UTFstring::offset(const int index) const
	{
		return m_string ? al_ustr_offset(m_string.get(), index) : 0;
	}

	bool UTFstring::next(int& i) const
	{
		return m_string ? al_ustr_next(m_string.get(), &i) : false;
	}

	bool UTFstring::prev(int& i) const
	{
		return m_string ? al_ustr_prev(m_string.get(), &i) : false;
	}

	int32_t UTFstring::get(const int pos) const
	{
		return m_string ? al_ustr_get(m_string.get(), pos) : 0;
	}

	int32_t UTFstring::operator[](const int pos) const
	{
		return get(pos);
	}

	int32_t UTFstring::get_next(int& pos) const
	{
		return m_string ? al_ustr_get_next(m_string.get(), &pos) : 0;
	}

	int32_t UTFstring::prev_get(int& pos) const
	{
		return m_string ? al_ustr_prev_get(m_string.get(), &pos) : 0;
	}

	bool UTFstring::insert_bytes(const size_t pos, const UTFstring& oth)
	{
		return m_string ? (oth.m_string ? (al_ustr_insert(m_string.get(), static_cast<int>(pos), oth.m_string.get())) : true) : false;
	}

	bool UTFstring::insert_bytes(const size_t pos, const std::string& oth)
	{
		return m_string ? (oth.length() ? (al_ustr_insert_cstr(m_string.get(), static_cast<int>(pos), oth.c_str())) : true) : false;
	}

	bool UTFstring::insert(const size_t pos, const UTFstring& oth)
	{
		return m_string ? (oth.m_string ? (al_ustr_insert(m_string.get(), al_ustr_offset(m_string.get(), static_cast<int>(pos)), oth.m_string.get())) : true) : false;
	}

	bool UTFstring::insert(const size_t pos, const std::string& oth)
	{
		return m_string ? (oth.length() ? (al_ustr_insert_cstr(m_string.get(), al_ustr_offset(m_string.get(), static_cast<int>(pos)), oth.c_str())) : true) : false;
	}

	bool UTFstring::append(const UTFstring& oth)
	{
		return m_string ? (oth.m_string ? (al_ustr_append(m_string.get(), oth.m_string.get())) : true) : false;
	}

	UTFstring& UTFstring::operator+=(const UTFstring& oth)
	{
		append(oth);
		return *this;
	}

	UTFstring UTFstring::operator+(const UTFstring& oth)
	{
		UTFstring cpy = *this;
		cpy.append(oth);
		return cpy;
	}

	bool UTFstring::append(const std::string& str)
	{
		return m_string ? (str.length() ? (al_ustr_append_cstr(m_string.get(), str.c_str())) : true) : false;
	}

	UTFstring& UTFstring::operator+=(const std::string& oth)
	{
		append(oth);
		return *this;
	}

	UTFstring UTFstring::operator+(const std::string& oth)
	{
		UTFstring cpy = *this;
		cpy.append(oth);
		return cpy;
	}

	bool UTFstring::append(const int32_t ch)
	{
		return m_string ? al_ustr_append_chr(m_string.get(), ch) : false;
	}

	bool UTFstring::remove_bytes(const size_t pos)
	{
		return m_string ? al_ustr_remove_chr(m_string.get(), static_cast<int>(pos)) : false;
	}

	bool UTFstring::remove(const size_t pos)
	{
		return m_string ? al_ustr_remove_chr(m_string.get(), al_ustr_offset(m_string.get(), static_cast<int>(pos))) : false;
	}

	bool UTFstring::remove_bytes(const size_t start_pos, const size_t end_pos)
	{
		return m_string ? al_ustr_remove_range(m_string.get(), static_cast<int>(start_pos), static_cast<int>(end_pos)) : false;
	}

	bool UTFstring::remove(const size_t start_pos, const size_t end_pos)
	{
		return m_string ? al_ustr_remove_range(m_string.get(), al_ustr_offset(m_string.get(), static_cast<int>(start_pos)), al_ustr_offset(m_string.get(), static_cast<int>(end_pos))) : false;
	}

	bool UTFstring::truncate_bytes(const size_t pos)
	{
		return m_string ? al_ustr_truncate(m_string.get(), static_cast<int>(pos)) : false;
	}

	bool UTFstring::truncate(const size_t pos)
	{
		return m_string ? al_ustr_truncate(m_string.get(), al_ustr_offset(m_string.get(), static_cast<int>(pos))) : false;
	}

	bool UTFstring::ltrim_white_space()
	{
		return m_string ? al_ustr_ltrim_ws(m_string.get()) : false;
	}

	bool UTFstring::rtrim_white_space()
	{
		return m_string ? al_ustr_rtrim_ws(m_string.get()) : false;
	}

	bool UTFstring::trim_white_space()
	{
		return m_string ? al_ustr_trim_ws(m_string.get()) : false;
	}

	bool UTFstring::assign(const UTFstring& oth)
	{
		if (!m_string) {
			m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_dup(oth.m_string.get()), [](ALLEGRO_USTR* s) {al_ustr_free(s); });
			return m_string.get();
		}
		return al_ustr_assign(m_string.get(), oth.m_string.get());
	}

	bool UTFstring::assign(const std::string& oth)
	{
		if (!m_string) {
			m_string = std::shared_ptr<ALLEGRO_USTR>(al_ustr_new_from_buffer(oth.c_str(), oth.length()), [](ALLEGRO_USTR* s) {al_ustr_free(s); });
			return m_string.get();
		}
		return al_ustr_assign_cstr(m_string.get(), oth.c_str());
	}

	UTFstring& UTFstring::operator=(const UTFstring& oth)
	{
		assign(oth);
		return *this;
	}

	UTFstring& UTFstring::operator=(const std::string& oth)
	{
		assign(oth);
		return *this;
	}

	size_t UTFstring::set_bytes(const size_t pos, const int32_t val)
	{
		return m_string ? al_ustr_set_chr(m_string.get(), static_cast<int>(pos), val) : 0;
	}

	size_t UTFstring::set(const size_t pos, const int32_t val)
	{
		return m_string ? al_ustr_set_chr(m_string.get(), al_ustr_offset(m_string.get(), static_cast<int>(pos)), val) : 0;
	}

	bool UTFstring::replace_bytes(const size_t pos_start, const size_t pos_end, const UTFstring& oth)
	{
		return m_string ? (oth.m_string ? (al_ustr_replace_range(m_string.get(), static_cast<int>(pos_start), static_cast<int>(pos_end), oth.m_string.get())) : true) : false;
	}

	bool UTFstring::replace(const size_t pos_start, const size_t pos_end, const UTFstring& oth)
	{
		return m_string ? (oth.m_string ? (al_ustr_replace_range(m_string.get(), al_ustr_offset(m_string.get(), static_cast<int>(pos_start)), al_ustr_offset(m_string.get(), static_cast<int>(pos_end)), oth.m_string.get())) : true) : false;
	}


	int UTFstring::find(const int32_t ch, const size_t offset, const bool offset_bytes, const bool return_bytes) const
	{
		if (!m_string) return -1;
		const auto val = al_ustr_find_chr(
			m_string.get(), 
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			ch
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFstring::rfind(const int32_t ch, const size_t offset, const bool offset_bytes, const bool return_bytes) const
	{
		if (!m_string) return -1;
		const auto val = al_ustr_rfind_chr(
			m_string.get(), 
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			ch
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}


	int UTFstring::find_set(const UTFstring& accept_any_of, const size_t offset, const bool offset_bytes, const bool return_bytes) const
	{
		if (!m_string) return -1;
		const auto val = al_ustr_find_set(
			m_string.get(), 
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			accept_any_of.m_string.get()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFstring::find_cset(const UTFstring& reject_any_of, const size_t offset, const bool offset_bytes, const bool return_bytes) const
	{
		if (!m_string) return -1;
		const auto val = al_ustr_find_set(
			m_string.get(), 
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			reject_any_of.m_string.get()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}


	int UTFstring::find_str(const UTFstring& needle, const size_t offset, const bool offset_bytes, const bool return_bytes) const
	{
		if (!m_string) return -1;
		const auto val = al_ustr_find_str(
			m_string.get(), 
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			needle.m_string.get()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFstring::find_str(const std::string& needle, const size_t offset, const bool offset_bytes, const bool return_bytes) const
	{
		if (!m_string) return -1;
		const auto val = al_ustr_find_cstr(
			m_string.get(), 
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			needle.c_str()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}


	int UTFstring::rfind_str(const UTFstring& needle, const size_t offset, const bool offset_bytes, const bool return_bytes) const
	{
		if (!m_string) return -1;
		const auto val = al_ustr_rfind_str(
			m_string.get(), 
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			needle.m_string.get()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}

	int UTFstring::rfind_str(const std::string& needle, const size_t offset, const bool offset_bytes, const bool return_bytes) const
	{
		if (!m_string) return -1;
		const auto val = al_ustr_find_cstr(
			m_string.get(), 
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			needle.c_str()
		);

		return return_bytes ? val : al_ustr_offset(m_string.get(), val);
	}


	bool UTFstring::find_replace(const UTFstring& search, const UTFstring& replace_with, const size_t offset, const bool offset_bytes)
	{
		if (!m_string) return -1;
		return al_ustr_find_replace(
			m_string.get(),
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			search.m_string.get(),
			replace_with.m_string.get()
		);
	}

	bool UTFstring::find_replace(const std::string& search, const std::string& replace_with, const size_t offset, const bool offset_bytes)
	{
		if (!m_string) return -1;
		return al_ustr_find_replace_cstr(
			m_string.get(),
			(offset_bytes ? static_cast<int>(offset) : al_ustr_offset(m_string.get(), static_cast<int>(offset))),
			search.c_str(),
			replace_with.c_str()
		);
	}

	int UTFstring::compare(const UTFstring& oth, const size_t codepoint_max) const
	{
		// Allegro: "Returns zero if the strings are equal, a positive number if us1 comes after us2, else a negative number."
		return
			(codepoint_max == static_cast<size_t>(-1)) ?
			(m_string ? (oth.m_string ? al_ustr_compare(m_string.get(), oth.m_string.get()) : 1) : (oth.m_string ? 1 : 0)) :
			(m_string ? (oth.m_string ? al_ustr_ncompare(m_string.get(), oth.m_string.get(), static_cast<int>(codepoint_max)) : 1) : (oth.m_string ? 1 : 0));
	}

	bool UTFstring::operator==(const UTFstring& oth) const
	{
		return m_string ? (oth.m_string ? al_ustr_equal(m_string.get(), oth.m_string.get()) : false) : (oth.m_string ? false : true);
	}

	bool UTFstring::operator!=(const UTFstring& oth) const
	{
		return m_string ? (oth.m_string ? (!al_ustr_equal(m_string.get(), oth.m_string.get())) : true) : (oth.m_string ? true : false);
	}

	int UTFstring::operator<=>(const UTFstring& oth) const
	{
		return compare(oth);
	}

	bool UTFstring::has_prefix(const UTFstring& oth) const
	{
		if (!m_string) return false;
		if (!oth.m_string) return true;
		return al_ustr_has_prefix(m_string.get(), oth.m_string.get());
	}

	bool UTFstring::has_suffix(const UTFstring& oth) const
	{
		if (!m_string) return false;
		if (!oth.m_string) return true;
		return al_ustr_has_suffix(m_string.get(), oth.m_string.get());
	}

	bool UTFstring::has_prefix(const std::string& oth) const
	{
		if (!m_string) return false;
		if (!oth.length()) return true;
		return al_ustr_has_prefix_cstr(m_string.get(), oth.c_str());
	}

	bool UTFstring::has_suffix(const std::string& oth) const
	{
		if (!m_string) return false;
		if (!oth.length()) return true;
		return al_ustr_has_suffix_cstr(m_string.get(), oth.c_str());
	}

	std::basic_string<uint16_t> UTFstring::encode_utf16() const
	{
		if (!m_string) return {};
		std::basic_string<uint16_t> utf16(al_ustr_size_utf16(m_string.get()), '\0');
		al_ustr_encode_utf16(m_string.get(), utf16.data(), utf16.size());
		return utf16;
	}

	std::string UTFstring::encode_utf8() const
	{
		return c_str();
	}

	size_t UTFstring::utf8_width(const int c)
	{
		return al_utf8_width(c);
	}

	size_t UTFstring::utf8_encode(char s[], const int c)
	{
		return al_utf8_encode(s, c);
	}

	size_t UTFstring::utf16_width(const int c)
	{
		return al_utf16_width(c);
	}

	size_t UTFstring::utf16_encode(uint16_t s[], const int c)
	{
		return al_utf16_encode(s, c);
	}


	std::string UTFstring::c_str() const
	{
		return m_string ? al_cstr(m_string.get()) : "";
	}

	UTFstring::operator std::string() const
	{
		return m_string ? al_cstr(m_string.get()) : "";
	}
}