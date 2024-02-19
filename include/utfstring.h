#pragma once

#include <allegro5/allegro.h>

#include <string>
#include <memory>

namespace AllegroCPP {

	class UTFstring {
		std::shared_ptr<ALLEGRO_USTR> m_string;
	public:
		UTFstring();
		UTFstring(const char*);
		UTFstring(const std::string&);
		UTFstring(const uint16_t* const);
		UTFstring(const UTFstring&);

		void to_buffer(char*, size_t);

		// literal characters
		UTFstring substr_bytes(const size_t start_pos, const size_t end_pos);
		// using al_ustr_offset
		UTFstring substr(const size_t start_pos, const size_t end_pos);

		// size in bytes
		size_t size() const;

		// size in symbols/code points
		size_t length() const;

		// get offset in bytes of symbol/code point at specific index. Negative counts backwards. If greater than string, gets last one
		int offset(const int index) const;

		// advance to next symbol/code point
		bool next(int&) const;
		// go back one symbol/code point
		bool prev(int&) const;

		// get symbol/code point at pos in symbols/code points
		int32_t get(const int pos) const;

		// get symbol/code point at pos in symbols/code points
		int32_t operator[](const int pos) const;

		// get symbol/code point at pos and do next at the same time
		int32_t get_next(int& pos) const;

		// do prev and get symbol/code point at pos at the same time
		int32_t prev_get(int& pos) const;

		// insert string at this position, in bytes
		bool insert_bytes(const size_t pos, const UTFstring&);
		bool insert_bytes(const size_t pos, const std::string&);

		// insert string at this position, in symbols/code points
		bool insert(const size_t pos, const UTFstring&);
		bool insert(const size_t pos, const std::string&);

		// insert at the end
		bool append(const UTFstring&);

		UTFstring& operator+=(const UTFstring& oth);
		UTFstring operator+(const UTFstring& oth);

		// insert at the end
		bool append(const std::string&);

		UTFstring& operator+=(const std::string& oth);
		UTFstring operator+(const std::string& oth);

		bool append(const int32_t);

		// remove at point in bytes
		bool remove_bytes(const size_t);

		// remove at point in symbols/code points
		bool remove(const size_t);

		// remove at point in bytes
		bool remove_bytes(const size_t start_pos, const size_t end_pos);

		// remove at point in symbols/code points
		bool remove(const size_t start_pos, const size_t end_pos);

		bool truncate_bytes(const size_t);

		bool truncate(const size_t);

		bool ltrim_white_space();
		bool rtrim_white_space();
		bool trim_white_space();

		bool assign(const UTFstring&);
		bool assign(const std::string&);

		UTFstring& operator=(const UTFstring&);
		UTFstring& operator=(const std::string&);

		size_t set_bytes(const size_t pos, const int32_t val);
		size_t set(const size_t pos, const int32_t val);

		bool replace_bytes(const size_t pos_start, const size_t pos_end, const UTFstring& oth);
		bool replace(const size_t pos_start, const size_t pos_end, const UTFstring& oth);

		// searching part
				
		int find(const int32_t ch, const size_t offset = 0, const bool offset_bytes = true, const bool return_bytes = true) const;		
		int rfind(const int32_t ch, const size_t offset = 0, const bool offset_bytes = true, const bool return_bytes = true) const;

		int find_set(const UTFstring& accept_any_of, const size_t offset = 0, const bool offset_bytes = true, const bool return_bytes = true) const;
		int find_cset(const UTFstring& reject_any_of, const size_t offset = 0, const bool offset_bytes = true, const bool return_bytes = true) const;

		// find full string in string
		int find_str(const UTFstring& needle, const size_t offset = 0, const bool offset_bytes = true, const bool return_bytes = true) const;
		// find full string in string
		int find_str(const std::string& needle, const size_t offset = 0, const bool offset_bytes = true, const bool return_bytes = true) const;
;
		// find full string in string
		int rfind_str(const UTFstring& needle, const size_t offset = 0, const bool offset_bytes = true, const bool return_bytes = true) const;
		// find full string in string
		int rfind_str(const std::string& needle, const size_t offset = 0, const bool offset_bytes = true, const bool return_bytes = true) const;

		// find all cases of string and replace with other string
		bool find_replace(const UTFstring& search, const UTFstring& replace_with, const size_t offset = 0, const bool offset_bytes = true);
		// find all cases of string and replace with other string
		bool find_replace(const std::string& search, const std::string& replace_with, const size_t offset = 0, const bool offset_bytes = true);

		int compare(const UTFstring& oth, const size_t codepoint_max = static_cast<size_t>(-1)) const;

		bool operator==(const UTFstring& oth) const;
		bool operator!=(const UTFstring& oth) const;
		int operator<=>(const UTFstring& oth) const;

		bool has_prefix(const UTFstring& oth) const;
		bool has_suffix(const UTFstring& oth) const;
		bool has_prefix(const std::string& oth) const;
		bool has_suffix(const std::string& oth) const;

		std::basic_string<uint16_t> encode_utf16() const;
		std::string encode_utf8() const;

		static size_t utf8_width(const int c);
		static size_t utf8_encode(char s[], const int c);
		static size_t utf16_width(const int c);
		static size_t utf16_encode(uint16_t s[], const int c);

		std::string c_str() const;
		operator std::string() const;

		const ALLEGRO_USTR* u_str() const;
		operator const ALLEGRO_USTR*() const;
	};
}