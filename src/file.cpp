#include "../include/file.h"

namespace AllegroCPP {

	File_shareable_ptr make_shareable_file(ALLEGRO_FILE* fp, std::function<void(ALLEGRO_FILE*)> destr)
	{
		using upt = std::unique_ptr<ALLEGRO_FILE, std::function<void(ALLEGRO_FILE*)>>;
		return std::shared_ptr<upt>(new upt(fp, destr));
	}

	File::File(const std::string& path, const std::string& mode)
		: m_curr_path(path)
	{
		if (path.empty() || mode.empty()) throw std::invalid_argument("Path or mode is empty!");
		if (!al_is_system_installed()) al_init();

		if (!(m_fp = make_shareable_file(al_fopen(path.c_str(), mode.c_str()), [](ALLEGRO_FILE* f) { al_fclose(f); }))) throw std::runtime_error("Could not open file!");
	}


	File::File(const std::string& path, const ALLEGRO_FILE_INTERFACE* interfac, const std::string& mode)
		: m_curr_path(path)
	{
		if (!interfac) throw std::invalid_argument("Interface is null!");
		if (path.empty() || mode.empty()) throw std::invalid_argument("Path or mode is empty!");
		if (!al_is_system_installed()) al_init();

		if (!(m_fp = make_shareable_file(al_fopen_interface(interfac, path.c_str(), mode.c_str()), [](ALLEGRO_FILE* f) { al_fclose(f); }))) throw std::runtime_error("Could not open file!");
	}

	File::File(const int fd, const std::string& mode)
	{
		if (fd < 0 || mode.empty()) throw std::invalid_argument("FD or mode is empty!");
		if (!al_is_system_installed()) al_init();		

		if (!(m_fp = make_shareable_file(al_fopen_fd(fd, mode.c_str()), [](ALLEGRO_FILE* f) { al_fclose(f); }))) throw std::runtime_error("Could not open FD!");
	}

	File::File(File& fp, size_t slice_initial_size, const std::string& mode)
		: m_curr_path(fp.m_curr_path)
	{
		if (!fp.m_fp) throw std::invalid_argument("File is empty!");
		if (mode.empty()) throw std::invalid_argument("Mode is invalid!");
		if (!al_is_system_installed()) al_init();

		if (!(m_fp = make_shareable_file(al_fopen_slice(fp.m_fp->get(), slice_initial_size, mode.c_str()), [](ALLEGRO_FILE* f) { al_fclose(f); }))) throw std::runtime_error("Could not open file!");
	}


	File::~File()
	{
		m_fp.reset();
	}

	bool File::empty() const
	{
		return !valid();
	}

	bool File::valid() const
	{
		return m_fp->get();
	}

	File::operator bool() const
	{
		return m_fp->get();
	}

	File::operator ALLEGRO_FILE* ()
	{
		return m_fp->get();
	}

	File::operator File_shareable_ptr() const
	{
		return m_fp;
	}

	File::File(File&& oth) noexcept
		: m_fp(std::exchange(oth.m_fp, {})), m_curr_path(std::move(oth.m_curr_path))
	{
	}

	void File::operator=(File&& oth) noexcept
	{
		m_fp.reset();
		m_curr_path = std::move(oth.m_curr_path);
		m_fp = std::exchange(oth.m_fp, {});
	}

	size_t File::read(void* dat, const size_t len)
	{
		return m_fp ? al_fread(m_fp->get(), dat, len) : 0;
	}

	size_t File::write(const void* dat, const size_t len)
	{
		return m_fp ? al_fwrite(m_fp->get(), dat, len) : 0;
	}

	bool File::flush()
	{
		return m_fp ? al_fflush(m_fp->get()) : false;
	}

	int64_t File::tell() const
	{
		return m_fp ? al_ftell((ALLEGRO_FILE*)m_fp->get()) : -1;
	}

	bool File::seek(const int64_t offset, const int whence)
	{
		return m_fp ? al_fseek(m_fp->get(), offset, whence) : false;
	}

	bool File::eof() const
	{
		return m_fp ? (al_feof((ALLEGRO_FILE*)m_fp->get()) || al_ferror((ALLEGRO_FILE*)m_fp->get()) != 0) : true;
	}

	bool File::has_error() const
	{
		return m_fp ? (al_ferror((ALLEGRO_FILE*)m_fp->get()) != 0) : false;
	}

	file_error_report File::get_error() const
	{
		return m_fp ? file_error_report{ al_ferrmsg((ALLEGRO_FILE*)m_fp->get()), al_ferror((ALLEGRO_FILE*)m_fp->get()) } : file_error_report{"null", 0};
	}

	int File::ungetc(const int c)
	{
		return m_fp ? al_fungetc(m_fp->get(), c) : -1;
	}

	int64_t File::size() const
	{
		return m_fp ? al_fsize((ALLEGRO_FILE*)m_fp->get()) : -1;
	}

	int File::getc()
	{
		return m_fp ? al_fgetc(m_fp->get()) : -1;
	}

	int File::putc(const int c)
	{
		return m_fp ? al_fputc(m_fp->get(), c) : -1;
	}

	int File::printformat(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		auto ret = al_vfprintf(m_fp->get(), format, args);
		va_end(args);
		return ret;
	}

	int File::vprintformat(const char* format, va_list args)
	{
		return al_vfprintf(m_fp->get(), format, args);
	}

	int16_t File::read16le()
	{
		return m_fp ? al_fread16le(m_fp->get()) : -1;
	}

	int16_t File::read16be()
	{
		return m_fp ? al_fread16be(m_fp->get()) : -1;
	}

	size_t File::write16le(int16_t c)
	{
		return m_fp ? al_fwrite16le(m_fp->get(), c) : -1;
	}

	size_t File::write16be(int16_t c)
	{
		return m_fp ? al_fwrite16le(m_fp->get(), c) : -1;
	}

	int32_t File::read32le()
	{
		return m_fp ? al_fread32le(m_fp->get()) : -1;
	}

	int32_t File::read32be()
	{
		return m_fp ? al_fread32be(m_fp->get()) : -1;
	}

	size_t File::write32le(int32_t c)
	{
		return m_fp ? al_fwrite32le(m_fp->get(), c) : -1;
	}

	size_t File::write32be(int32_t c)
	{
		return m_fp ? al_fwrite32le(m_fp->get(), c) : -1;
	}

	File& File::operator<<(const char* val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		this->write(val, strlen(val));
		return *this;
	}

	File& File::operator<<(const std::string& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		this->write(val.data(), val.size());
		return *this;
	}

	File& File::operator<<(const std::vector<char>& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		this->write(val.data(), val.size());
		return *this;
	}

	File& File::operator<<(bool val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const std::string str = (val ? "1" : "0");
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(short val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(unsigned short val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(int val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(unsigned int val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(long val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(unsigned long val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(long long val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(unsigned long long val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(float val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(double val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(long double val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const auto str = std::to_string(val);
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(std::streambuf* sb)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");		
		std::stringstream ss;
		ss << sb;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(std::ostream& (*pf)(std::ostream&))
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(std::ios& (*pf)(std::ios&))
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator<<(std::ios_base& (*pf)(std::ios_base&))
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::stringstream ss;
		ss << pf;
		const auto str = ss.str();
		this->write(str.data(), str.size());
		return *this;
	}

	File& File::operator>>(std::string& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		val = this->gets(static_cast<size_t>(1) << 12);
		return *this;
	}

	File& File::operator>>(std::vector<char>& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		const std::string _tmp = this->gets(static_cast<size_t>(1) << 12);
		val.assign(std::move_iterator(_tmp.begin()), std::move_iterator(_tmp.end()));
		return *this;
	}

	File& File::operator>>(bool& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		char _tmp{};
		val = (this->read(&_tmp, sizeof(_tmp)) == 0) ? false : (_tmp == '1');
		return *this;
	}

	File& File::operator>>(short& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && (std::isdigit(_tmp) || _tmp == '-') && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
#ifdef _WIN32
			if (sscanf_s(_buf.data(), "%hd", &val) == 1) return *this;
			val = 0;
#else
			if (sscanf(_buf.data(), "%hd", &val) == 1) return *this;
			val = 0;
#endif
		}
		return *this;
	}

	File& File::operator>>(unsigned short& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
#ifdef _WIN32
			if (sscanf_s(_buf.data(), "%hu", &val) == 1) return *this;
			val = 0;
#else
			if (sscanf(_buf.data(), "%hu", &val) == 1) return *this;
			val = 0;
#endif
		}
		return *this;
	}

	File& File::operator>>(int& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stoi(_buf);
		}
		return *this;
	}

	File& File::operator>>(unsigned int& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
#ifdef _WIN32
			if (sscanf_s(_buf.data(), "%u", &val) == 1) return *this;
			val = 0;
#else
			if (sscanf(_buf.data(), "%u", &val) == 1) return *this;
			val = 0;
#endif
		}
		return *this;
	}

	File& File::operator>>(long& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stol(_buf);
		}
		return *this;
	}

	File& File::operator>>(unsigned long& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stoul(_buf);
		}
		return *this;
	}

	File& File::operator>>(long long& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stoll(_buf);
		}
		return *this;
	}

	File& File::operator>>(unsigned long long& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stoull(_buf);
		}
		return *this;
	}

	File& File::operator>>(float& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stof(_buf);
		}
		return *this;
	}

	File& File::operator>>(double& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stod(_buf);
		}
		return *this;
	}

	File& File::operator>>(long double& val)
	{
		if (!m_fp) throw std::runtime_error("Can't write on null/empty file!");
		std::string _buf;
		for (char _tmp; this->read(&_tmp, sizeof(_tmp)) != 0 && std::isdigit(_tmp) && !this->eof();) _buf += _tmp;
		if (_buf.empty()) val = 0;
		else {
			val = std::stold(_buf);
		}
		return *this;
	}


	std::string File::gets(const size_t max)
	{
		if (!m_fp) return {};
		std::string str(max, '\0');
		al_fgets(m_fp->get(), str.data(), str.size());
		size_t exp_siz = strnlen(str.data(), max);
		if (exp_siz < str.size()) str.resize(exp_siz);
		return str;
	}

	char* File::gets(char* const buf, const size_t max)
	{
		return m_fp ? al_fgets(m_fp->get(), buf, max) : nullptr;
	}

	std::shared_ptr<ALLEGRO_USTR> File::get_ustr()
	{
		if (!m_fp) return nullptr;
		return std::shared_ptr<ALLEGRO_USTR>(al_fget_ustr(m_fp->get()), [](ALLEGRO_USTR* u) { al_ustr_free(u); });
	}

	bool File::puts(char const* str)
	{
		return m_fp ? (al_fputs(m_fp->get(), str) >= 0) : false;
	}

	bool File::puts(const std::string& str)
	{
		return m_fp ? (al_fputs(m_fp->get(), str.c_str()) >= 0) : false;
	}

	const std::string& File::get_filepath() const
	{
		return m_curr_path;
	}

	File File::clone_for_read() const
	{
		const auto& path = get_filepath();
		return File(path, "rb");
	}

	void File::close()
	{
		m_fp.reset();
	}

	//ALLEGRO_FILE* File::drop()
	//{
	//	ALLEGRO_FILE* nf = m_fp;
	//	m_fp = nullptr;
	//	return m_fp.;
	//}

	File_tmp::File_tmp(const std::string& tmppath, const std::string& mode)
	{
		if (tmppath.empty()) throw std::invalid_argument("File format is empty!");
		if (!al_is_system_installed()) al_init();

		ALLEGRO_PATH* tmpptr = al_create_path(nullptr);
		auto* fpp = al_make_temp_file(tmppath.c_str(), &tmpptr);
		if (!fpp) throw std::runtime_error("Could not open temp file!");
		m_curr_path = al_path_cstr(tmpptr, ALLEGRO_NATIVE_PATH_SEP);
		al_destroy_path(tmpptr);
		al_fclose(fpp);

		if (!(m_fp = make_shareable_file(al_fopen(m_curr_path.c_str(), mode.c_str()), [path = m_curr_path](ALLEGRO_FILE* f) { al_fclose(f); if (!path.empty()) { std::remove(path.c_str()); } }))) {
			al_destroy_path(tmpptr);
			throw std::runtime_error("Could not open temp file!");
		}

		//m_curr_path = al_path_cstr(tmpptr, ALLEGRO_NATIVE_PATH_SEP);
		//al_destroy_path(tmpptr);
	}

	File_tmp::~File_tmp()
	{
		m_fp.reset();
	}

	File_tmp::File_tmp(File_tmp&& oth) noexcept 
		: File(std::move(oth))
	{
	}

	void File_tmp::operator=(File_tmp&& oth) noexcept
	{
		this->File::operator=(std::move(oth));
	}

	File_memory::File_memory(size_t memlen)
	{
		if (memlen == 0) throw std::invalid_argument("Memory size can't be zero!");
		if (!al_is_system_installed()) al_init();

		if (!(m_mem = (char*)al_malloc(memlen))) throw std::runtime_error("Can't alloc!");
		m_fp = make_shareable_file(al_open_memfile(m_mem, memlen, "wb+"), [this](ALLEGRO_FILE* f) { al_fclose(f);  al_free(m_mem); });
	}

	File_memory::~File_memory()
	{
		this->File::~File(); // close file FIRST
	}

	File_memory::File_memory(File_memory&& oth) noexcept
		: File(std::move(oth)), m_mem(std::exchange(oth.m_mem, nullptr))
	{
		//oth.m_mem = nullptr;
	}

	void File_memory::operator=(File_memory&& oth) noexcept
	{
		this->File::operator=(std::move(oth));
		if (m_mem) al_free(m_mem);
		m_mem = std::exchange(oth.m_mem, nullptr);
	}

	namespace _socketmap {

		static std::string addrInfoToIP(SocketAddrInfo* pai) {
			if (pai->ai_family == AF_INET) {
				struct sockaddr_in* psai = (struct sockaddr_in*)pai->ai_addr;
				char ip[INET_ADDRSTRLEN];
				if (inet_ntop(pai->ai_family,  &(psai->sin_addr), ip, INET_ADDRSTRLEN) != NULL)
					return std::string(ip);
			}
			else if (pai->ai_family == AF_INET6) {
				struct sockaddr_in6* psai = (struct sockaddr_in6*)pai->ai_addr;
				char ip[INET6_ADDRSTRLEN];
				if (inet_ntop(pai->ai_family, &(psai->sin6_addr), ip, INET6_ADDRSTRLEN) != NULL)
					return std::string(ip);
			}
			return "";
		};

		non_implemented::non_implemented(const char* s) : m_msg(s)
		{}
		non_implemented::non_implemented(const std::string& s) : m_msg(s)
		{}

		const char* non_implemented::what() const 
		{
			return m_msg.c_str();
		}

		socket_user_data::_eachsock::_eachsock(SocketType a, SocketAddrInfo b, socket_type c, std::string d)
			: sock(a), info(b), type(c), src_ip(d)
		{
		}

		bool socket_user_data::has_host() const
		{
			for (const auto& i : m_socks) { if (i.type == socket_type::TCP_HOST || i.type == socket_type::UDP_HOST) return true; }
			return false;
		}

		void socket_user_data::close_auto()
		{
			for (auto& i : m_socks) { if (i.type != socket_type::UDP_HOST_CLIENT) closeSocket(i.sock); }
			m_socks.clear();
		}

		void* sock_open(const char* nadd, const char* plen)
		{
			const socket_config* __conf = (socket_config*)nadd;
			const uint64_t* __conflen = (uint64_t*)plen;

			if (!__conf || !__conflen || (*__conflen) != sizeof(socket_config)) throw std::invalid_argument("Expected specific input, got something else");

			const socket_config& theconf = *__conf;
			if (theconf.prealloc) return theconf.prealloc;

			socket_user_data* sud = new socket_user_data();
			if (!sud) { throw std::bad_alloc(); }

			sud->badflag = 0;
			sud->original_addr = theconf.addr;
			sud->original_port = theconf.port;
			socket_type type = theconf.host ? (theconf.protocol == SOCK_STREAM ? socket_type::TCP_HOST : socket_type::UDP_HOST) : (theconf.protocol == SOCK_STREAM ? socket_type::TCP_CLIENT : socket_type::UDP_CLIENT);

			SocketAddrInfo* AddrInfo = nullptr;
			SocketAddrInfo Hints{};

			char Port[8]{};
#ifdef _WIN32
			sprintf_s(Port, "%hu", theconf.port);
			ZeroMemory(&Hints, sizeof(Hints));
#else
			sprintf(Port, "%hu", theconf.port);
			memset(&Hints, 0, sizeof(Hints));
#endif

			Hints.ai_socktype = theconf.protocol;
			Hints.ai_family = theconf.family;
			if (theconf.host) Hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;

			if (getaddrinfo(theconf.addr.size() ? theconf.addr.c_str() : nullptr, Port, &Hints, &AddrInfo) != 0) {
				sud->badflag |= static_cast<int32_t>(socket_errors::GETADDR_FAILED);
				return sud;
			}

			int i = 0;
			for (SocketAddrInfo* AI = AddrInfo; AI != nullptr && i != FD_SETSIZE; AI = AI->ai_next)
			{
				if (theconf.host && (AI->ai_family != PF_INET) && (AI->ai_family != PF_INET6)) continue;

				SocketType sock = SocketInvalid;

				if (!SocketGood(sock = ::socket(AI->ai_family, AI->ai_socktype, AI->ai_protocol))) continue;

				if (theconf.host) {
					int on = 1;
					setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

					if (::bind(sock, AI->ai_addr, (int)AI->ai_addrlen) == SocketError) {
						closeSocket(sock);
						continue;
					}

					// Specific for TCP
					if (theconf.protocol == SOCK_STREAM) {
						if (::listen(sock, 5) == SocketError) {
							closeSocket(sock);
							continue;
						}
					}

					sud->m_socks.push_back({ sock, *AI, type, addrInfoToIP(AI) });
				}
				else {
					if (::connect(sock, AI->ai_addr, (int)AI->ai_addrlen) == SocketError) {
						closeSocket(sock);
						continue;
					}

					if (theconf.opt_broadcast && theconf.protocol == SOCK_DGRAM) { // broadcaster
						int enabled = 1;
						setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(enabled));
					}

					{	// no blocking
						u_long u = 1;
						ioctlSocket(sock, FIONBIO, &u);
					}

					if (AI == nullptr) {
						sud->badflag |= static_cast<int32_t>(socket_errors::GETADDR_FAILED);
						freeaddrinfo(AddrInfo);
						return sud;
					}

					SocketAddrInfo cpy = *AI;

					sud->m_socks.push_back({ sock, cpy, type, addrInfoToIP(AI) });

					freeaddrinfo(AddrInfo);
					return sud;
				}

				++i;
			}

			freeaddrinfo(AddrInfo);
			if (sud->m_socks.empty()) sud->badflag |= static_cast<int32_t>(socket_errors::ADDR_CANT_FIND);
			return sud;
		}

		bool sock_close(ALLEGRO_FILE* fp)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			if (!sud) return false;
			sud->close_auto();
			delete sud;
			return true;
		}

		size_t sock_read(ALLEGRO_FILE* fp, void* ptr, size_t size)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			if (!sud) return 0;
			if (sud->m_socks.empty()) { sud->badflag |= static_cast<int32_t>(socket_errors::MODE_WAS_INVALID); return 0; }
			int res = 0;

			if (sud->has_host()) {
				if (size != sizeof(new_socket_user_data)) { sud->badflag |= static_cast<int32_t>(socket_errors::HOST_PTR_RECV_FAIL); return 0; }

				new_socket_user_data& oths = *(new_socket_user_data*)ptr;
				const auto ittrg = sock_listen(sud->m_socks, oths.timeout);
				if (ittrg == sud->m_socks.end()) { sud->badflag |= static_cast<int32_t>(socket_errors::RECV_FAILED); return 0; }

				socklen_t _temp_len = sizeof(SocketStorage);
				SocketAddrInfo trigginfo{};

				switch (ittrg->type) {
				case socket_type::TCP_HOST:
				{
					SocketType accep = ::accept(ittrg->sock, (sockaddr*)&trigginfo, &_temp_len);
					if (!SocketGood(accep)) { sud->badflag |= static_cast<int32_t>(socket_errors::RECV_FAILED); return 0; }

					{	// no blocking
						u_long u = 1;
						ioctlSocket(accep, FIONBIO, &u);
					}

					oths.ptr->m_socks.push_back({ accep, trigginfo, socket_type::TCP_CLIENT, ittrg->src_ip });
					oths.ptr->badflag = 0;
					res = sizeof(socket_user_data);
				}
					break;
				case socket_type::UDP_HOST:
				{
					char byte{};
					// sock_listen triggers on 1 or bigger, so no lock expected here lmao
					res = ::recvfrom(ittrg->sock, &byte, 1, MSG_PEEK, (sockaddr*)&trigginfo, &_temp_len);

					if (res < 0 && theSocketError != SocketBUFFERSMALL) {
						sud->badflag |= static_cast<int32_t>(socket_errors::RECV_FAILED);
						oths.ptr->badflag |= static_cast<int32_t>(socket_errors::RECV_FAILED);
						res = 0;
					}
					else if (res == 0) { return 0; } // empty packet?
					else { // res > 0 or not SocketBUFFERSMALL (expected if package is > 1 because hackz)
						{	// no blocking
							u_long u = 1;
							ioctlSocket(ittrg->sock, FIONBIO, &u);
						}
						oths.ptr->m_socks.push_back({ ittrg->sock, trigginfo, socket_type::UDP_HOST_CLIENT, ittrg->src_ip });
						oths.ptr->badflag = 0;
						res = sizeof(socket_user_data);
					}
					break;
				}
					break;
				default:
					sud->badflag |= static_cast<int32_t>(socket_errors::SOCKET_INVALID);
					return 0;
				}
			}
			else {
				auto& curr = sud->m_socks[0];

				{
					int err = 0;
					socklen_t len = sizeof(err);
					if (getsockopt(curr.sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len) != 0 || err != 0)
					{
						sud->badflag |= static_cast<int32_t>(socket_errors::SOCKET_HAD_ERROR);
						return 0;
					}
				}

				switch (curr.type) {
				case socket_type::TCP_CLIENT:
				{
					res = ::recv(curr.sock, (char*)ptr, static_cast<int>(size), 0);
					if (res < 0) { sud->badflag |= static_cast<int32_t>(socket_errors::RECV_FAILED); }
					else if (res == 0) { sud->badflag |= static_cast<int32_t>(socket_errors::CLOSED); }
				}
				break;
				case socket_type::UDP_CLIENT:
				case socket_type::UDP_HOST_CLIENT:
				{
					socklen_t _temp_len = sizeof(SocketStorage);
					res = ::recvfrom(curr.sock, (char*)ptr, static_cast<int>(size), 0, (sockaddr*)&curr.info, &_temp_len);
					if (res < 0) { sud->badflag |= static_cast<int32_t>(socket_errors::RECV_FAILED); }
					else if (res == 0) { sud->badflag |= static_cast<int32_t>(socket_errors::CLOSED); }
				}
				break;
				default:
					sud->badflag |= static_cast<int32_t>(socket_errors::SOCKET_INVALID);
					return 0;
				}
			}

			return res > 0 ? static_cast<size_t>(res) : 0;
		}

		size_t sock_write(ALLEGRO_FILE* fp, const void* ptr, size_t size)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			if (!sud) return 0;
			if (sud->m_socks.empty() || sud->has_host()) { sud->badflag |= static_cast<int32_t>(socket_errors::MODE_WAS_INVALID); return 0; }
			int res = 0;

			auto& curr = sud->m_socks[0];

			{
				int err = 0;
				socklen_t len = sizeof(err);
				if (getsockopt(curr.sock, SOL_SOCKET, SO_ERROR, (char*)&err, &len) != 0 || err != 0)
				{
					sud->badflag |= static_cast<int32_t>(socket_errors::SOCKET_HAD_ERROR);
					return 0;
				}
			}

			switch (curr.type) {
			case socket_type::TCP_CLIENT:
			case socket_type::UDP_CLIENT:
				res = ::send(curr.sock, (char*)ptr, static_cast<int>(size), 0);
				if (res <= 0) { sud->badflag |= static_cast<int32_t>(socket_errors::SEND_FAILED); }
				break;
			case socket_type::UDP_HOST_CLIENT:
				res = ::sendto(curr.sock, (char*)ptr, static_cast<int>(size), 0, (sockaddr*)&curr.info, sizeof(curr.info));
				if (res <= 0) { sud->badflag |= static_cast<int32_t>(socket_errors::SEND_FAILED); }
				break;
			default:
				sud->badflag |= static_cast<int32_t>(socket_errors::MODE_WAS_INVALID);
				return 0;
			}

			return res > 0 ? static_cast<size_t>(res) : 0;
		}

		bool sock_flush(ALLEGRO_FILE* fp)
		{
			throw non_implemented("Flush was not implemented for Sockets.");
			return false; // invalid operation
		}

		int64_t sock_tell(ALLEGRO_FILE* fp)
		{
			throw non_implemented("Tell was not implemented for Sockets.");
			return 0; // invalid operation
		}

		bool sock_seek(ALLEGRO_FILE* fp, int64_t offset, int whence)
		{
			throw non_implemented("Seek was not implemented for Sockets.");
			return false; // invalid operation
		}

		bool sock_eof(ALLEGRO_FILE* fp)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			return (!sud || sud->m_socks.empty() || sud->badflag > 0);
		}

		int sock_error(ALLEGRO_FILE* fp)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			return sud ? sud->badflag : 1;
		}

		const char* sock_errmsg(ALLEGRO_FILE* fp)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			if (!sud) return "Internal pointer is NULL";

			if (sud->badflag & static_cast<int32_t>(socket_errors::SOCKET_INVALID))	    return "Socket is in a invalid state";
			if (sud->badflag & static_cast<int32_t>(socket_errors::RECV_FAILED))	    return "Socket failed to recv at least once (-1)";
			if (sud->badflag & static_cast<int32_t>(socket_errors::SEND_FAILED))	    return "Socket failed to send at least once (-1)";
			if (sud->badflag & static_cast<int32_t>(socket_errors::GETADDR_FAILED))	    return "Getaddrinfo failed to find a valid option";
			if (sud->badflag & static_cast<int32_t>(socket_errors::ADDR_CANT_FIND))	    return "Could not find a valid AddrInfo";
			if (sud->badflag & static_cast<int32_t>(socket_errors::MODE_WAS_INVALID))   return "There was no socket set";
			if (sud->badflag & static_cast<int32_t>(socket_errors::HOST_PTR_RECV_FAIL)) return "On recv the pointer was invalid";
			if (sud->badflag & static_cast<int32_t>(socket_errors::CLOSED))				return "Socket was closed (disconnected)";
			return "Unknown";
		}

		void sock_clearerr(ALLEGRO_FILE* fp)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			if (sud) sud->badflag = 0;
		}

		int sock_ungetc(ALLEGRO_FILE* fp, int c)
		{
			throw non_implemented("Ungetc was not implemented for Sockets.");
			return 0; // invalid operation
		}

		off_t sock_size(ALLEGRO_FILE* fp)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			return sud ? static_cast<off_t>(sud->m_socks.size()) : 0;
		}

		std::vector<socket_user_data::_eachsock>::const_iterator sock_listen(const std::vector<socket_user_data::_eachsock>& servers, const long timeout)
		{
			std::vector<SocketType> _tmp;
			for (const auto& i : servers) _tmp.push_back(i.sock);
			SocketType _sock = sock_listen(_tmp, timeout);
			if (!SocketGood(_sock)) return servers.end();
			return std::find_if(servers.begin(), servers.end(), [&](const socket_user_data::_eachsock& e) { return e.sock == _sock; });			
		}

		SocketType sock_listen(const std::vector<SocketType>& servers, const long timeout)
		{
			if (servers.empty()) return SocketInvalid;

			const unsigned long nfds = static_cast<unsigned long>(servers.size());

			std::unique_ptr<SocketPollFD[]> pul(new SocketPollFD[nfds]);
			memset(pul.get(), 0, sizeof(SocketPollFD) * nfds);

			for (size_t p = 0; p < nfds; p++) {
				pul[p].events = SocketPOLLIN; // SocketPOLLIN = check data to read
				pul[p].fd = servers[p];
			}

			int res = pollSocket(pul.get(), nfds, timeout);

			if (res < 0) {
				return SocketInvalid;
			}

			for (size_t pp = 0; pp < nfds; pp++) {
				auto& it = pul[pp];
				if (it.revents != SocketPOLLIN) continue; // not "read data available"
				if (std::find(servers.begin(), servers.end(), it.fd) != servers.end()) return it.fd; // this is the one.
			}

			return SocketInvalid;
		}

		void setsocktimeout_auto(SocketType sock, unsigned long ms)
		{
			if (!SocketGood(sock)) return;
			// LINUX
#ifdef _WIN32
			// WINDOWS
			DWORD timeout = static_cast<DWORD>(ms);
			::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
			struct timeval tv;
			tv.tv_sec = ms / 1000000;
			tv.tv_usec = ms % 1000000;
			::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
#endif
		}

#ifdef _WIN32 
		_FileSocket::_winsock_start _FileSocket::__winsock;

		_FileSocket::_winsock_start::_winsock_start()
		{
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) throw std::runtime_error("Can't start WSA");
		}

		_FileSocket::_winsock_start::~_winsock_start()
		{
			WSACleanup();
		}
#endif

		void _FileSocket::set(_socketmap::socket_user_data* absorb)
		{
			if (!absorb) return; // invalid, got null, like empty file or closed.

			_socketmap::socket_config conf;
			uint64_t len = sizeof(conf);

			conf.prealloc = absorb;

			m_fp = make_shareable_file(al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len),
				[](ALLEGRO_FILE* f) { al_fclose(f); });

			if (!m_fp) throw std::runtime_error("Could not create FileSocket");
		}

		_FileSocket::_FileSocket() 
		{
			m_fp = nullptr; // no file, nothing, empty
		}

		_FileSocket::_FileSocket(_FileSocket&& oth) noexcept
			: File(std::move(oth))
		{
		}

		void _FileSocket::operator=(_FileSocket&& oth) noexcept
		{
			this->File::operator=(std::move(oth));
		}

		bool _FileSocket::empty() const
		{
			if (this->File::empty()) return true;
			return eof();
		}

		bool _FileSocket::valid() const
		{
			return !has_error();
		}

		const std::string& _FileSocket::get_filepath() const
		{
			if (!m_fp) {
				static std::string empty;
				return empty;
			}
			_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());
			return sod->original_addr.length() == 0 ? get_ipaddr_of(0) : sod->original_addr;
		}

		const std::string& _FileSocket::get_ipaddr_of(size_t off) const
		{
			static std::string empty;
			if (!m_fp) {
				return empty;
			}
			_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());
			if (sod->m_socks.size() <= off) return empty;
			return sod->m_socks[off].src_ip;
		}

		_FileSocket::operator bool() const
		{
			return !has_error();
		}
		
		std::string _FileSocket::gets(const size_t max)
		{
			if (!m_fp) return {};
			_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());
			if (!sod || sod->badflag || sod->has_host()) return {};
			auto& curr = sod->m_socks[0];
			int res = 0;
			switch (curr.type) {
			case socket_type::TCP_CLIENT:
				return this->File::gets(max);
			case socket_type::UDP_CLIENT:
			case socket_type::UDP_HOST_CLIENT:
			{
				std::string str(max, '\0');
				size_t tot = al_fread(m_fp->get(), str.data(), str.size());
				str.resize(tot);
				return str;
			}
			default:
				return {};
			}
		}

		char* _FileSocket::gets(char* const buf, const size_t max)
		{
			if (!m_fp) return {};
			_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());
			if (!sod || sod->badflag || sod->has_host()) return {};
			auto& curr = sod->m_socks[0];
			switch (curr.type) {
			case socket_type::TCP_CLIENT:
				return this->File::gets(buf, max);
			case socket_type::UDP_CLIENT:
			case socket_type::UDP_HOST_CLIENT:
			{
				al_fread(m_fp->get(), buf, max);
				return buf;
			}
			default:
				return {};
			}
		}

		std::shared_ptr<ALLEGRO_USTR> _FileSocket::get_ustr()
		{
			// std::shared_ptr<ALLEGRO_USTR>(al_fget_ustr(m_fp->get()), [](ALLEGRO_USTR* u) { al_ustr_free(u); });

			if (!m_fp) return {};
			_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());
			if (!sod || sod->badflag || sod->has_host()) return {};
			auto& curr = sod->m_socks[0];
			// hopefully packet is 128 * N (reads of 128)
			return this->File::get_ustr();

			//switch (curr.type) {
			//case socket_type::TCP_CLIENT:
			//	return this->File::get_ustr();
			//case socket_type::UDP_CLIENT:
			//case socket_type::UDP_HOST_CLIENT:
			//	throw std::runtime_error("Operation not available for UDP. Sorry.");
			//default:
			//	return {};
			//}
		}

		bool _FileSocket::puts(char const* str)
		{
			if (!m_fp) return false;
			return al_fwrite(m_fp->get(), str, strlen(str)) > 0;
		}

		bool _FileSocket::puts(const std::string& str)
		{
			if (!m_fp) return false;
			return al_fwrite(m_fp->get(), str.data(), str.size()) == str.size();
		}

		void _FileSocket::set_broadcast(const bool b)
		{
			if (!m_fp) return;
			_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());

			int enabled = b ? 1 : 0;
			for (auto& i : sod->m_socks) {
				setsockopt(i.sock, SOL_SOCKET, SO_BROADCAST, (char*)&enabled, sizeof(enabled));
			}
		}

	}

	File_client::File_client(_socketmap::socket_user_data* absorb)
	{
		set(absorb);
	}

	File_client::File_client(const std::string& addr, const uint16_t port, const int protocol, const int family, const bool broadcast)
	{
		_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.addr = addr;
		conf.protocol = protocol;
		conf.family = family;
		conf.port = port;
		conf.host = false;
		conf.opt_broadcast = broadcast;

		m_fp = make_shareable_file(al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len), 
			[](ALLEGRO_FILE* f) { al_fclose(f); });

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	File_client::File_client(const std::string& addr, const uint16_t port, const file_protocol protocol, const file_family family, const bool broadcast)
	{
		_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.addr = addr;
		conf.protocol = static_cast<int>(protocol);
		conf.family = static_cast<int>(family);
		conf.port = port;
		conf.host = false;
		conf.opt_broadcast = broadcast;

		m_fp = make_shareable_file(al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len),
			[](ALLEGRO_FILE* f) { al_fclose(f); });

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	File_client::File_client(File_client&& oth) noexcept
		: _FileSocket(std::move(oth))
	{
	}

	void File_client::operator=(File_client&& oth) noexcept
	{
		this->_FileSocket::operator=(std::move(oth));
	}

	bool File_client::set_timeout_read(const unsigned long ms)
	{
		if (!m_fp) return false;
		_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());
		if (!sod) return false;

		for (auto& i : sod->m_socks) {
			_socketmap::setsocktimeout_auto(i.sock, 1000);
		}
		return sod->m_socks.size() > 0;
	}

	File_host::File_host(const uint16_t port, const int protocol, const int family)
	{
		if (family == PF_UNSPEC) {
			File_host v4(port, protocol, PF_INET);
			File_host v6(port, protocol, PF_INET6);
			combine(std::move(v4));
			combine(std::move(v6));
			return;
		}

		_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.protocol = protocol;
		conf.family = family;
		conf.port = port;
		conf.host = true;

		m_fp = make_shareable_file(al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len),
			[](ALLEGRO_FILE* f) { al_fclose(f); });

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	File_host::File_host(const uint16_t port, const file_protocol protocol, const file_family family)
	{
		_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.protocol = static_cast<int>(protocol);
		conf.family = static_cast<int>(family);
		conf.port = port;
		conf.host = true;

		m_fp = make_shareable_file(al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len),
			[](ALLEGRO_FILE* f) { al_fclose(f); });

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	File_host::File_host(File_host&& oth) noexcept
		: _FileSocket(std::move(oth))
	{
	}

	void File_host::operator=(File_host&& oth) noexcept
	{
		this->_FileSocket::operator=(std::move(oth));
	}

	bool File_host::combine(File_host&& oth)
	{
		if (!m_fp) {
			*this = std::move(oth);
			return true;
		}
		if (!oth.m_fp) return false;
		_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());
		_socketmap::socket_user_data* sod_them = (_socketmap::socket_user_data*)al_get_file_userdata(oth.m_fp->get());
		if (!sod || !sod_them || !sod->has_host() || !sod_them->has_host()) return false;
		sod->m_socks.insert(sod->m_socks.end(), std::move_iterator(sod_them->m_socks.begin()), std::move_iterator(sod_them->m_socks.end()));
		sod_them->m_socks.clear();
		//al_fclose(oth.m_fp);
		//oth.m_fp = nullptr;
		oth.m_fp.reset();
		return true;
	}

	File_client File_host::listen(const long timeout)
	{
		if (!m_fp) throw std::runtime_error("Invalid state: null");
		_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp->get());
		if (!sod || !sod->has_host()) throw std::runtime_error("Invalid state: it's not a host?!");

		_socketmap::new_socket_user_data nsud;
		nsud.ptr = new _socketmap::socket_user_data;
		if (!nsud.ptr) throw std::bad_alloc();
		nsud.timeout = timeout;

		size_t confirm = al_fread(m_fp->get(), (void*)&nsud, sizeof(_socketmap::new_socket_user_data));
		if (confirm != sizeof(_socketmap::socket_user_data)) {
			delete nsud.ptr;
			return File_client(nullptr);
		}

		return File_client(nsud.ptr);
	}

#ifdef _WIN32
	File_memory file_load_resource_name_in_memory(int defined_name, const char* type_name)
	{
		HRSRC myResource = ::FindResourceA(NULL, MAKEINTRESOURCEA(defined_name), type_name);
		if (!myResource) throw std::runtime_error("Cannot find resource!");

		DWORD myResourceSize = ::SizeofResource(NULL, myResource); // bytes
		HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
		if (!myResourceData) throw std::runtime_error("Resource is empty?!");

		char* pMyBinaryData = (char*) ::LockResource(myResourceData);

		File_memory ofp(myResourceSize);
		if (!ofp) {
			UnlockResource(myResourceData);
			throw std::runtime_error("Cannot alloc memory for resource load!");
		}

		ofp.write(pMyBinaryData, myResourceSize);

		UnlockResource(myResourceData);

		ofp.seek(0, ALLEGRO_SEEK_SET);
		return ofp;
	}

	File_tmp file_load_resource_name_to_temp_file(int defined_name, const char* type_name, const std::string& file_extension_including_dot)
	{
		File_tmp ofp("XXXXXXXXX" + file_extension_including_dot);
		if (!ofp) return ofp;

		HRSRC myResource = ::FindResourceA(NULL, MAKEINTRESOURCEA(defined_name), type_name);
		if (!myResource) throw std::runtime_error("Cannot find resource!");

		DWORD myResourceSize = ::SizeofResource(NULL, myResource); // bytes
		HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
		if (!myResourceData) throw std::runtime_error("Resource is empty?!");

		char* pMyBinaryData = (char*) ::LockResource(myResourceData);

		ofp.write(pMyBinaryData, myResourceSize);
		ofp.flush();

		UnlockResource(myResourceData);

		ofp.seek(0, ALLEGRO_SEEK_SET);
		return ofp;
	}

	File file_load_resource_name_to_file(const char* file_name, int defined_name, const char* type_name)
	{
		File ofp(file_name);
		if (!ofp) return ofp;

		HRSRC myResource = ::FindResourceA(NULL, MAKEINTRESOURCEA(defined_name), type_name);
		if (!myResource) throw std::runtime_error("Cannot find resource!");

		DWORD myResourceSize = ::SizeofResource(NULL, myResource); // bytes
		HGLOBAL myResourceData = ::LoadResource(NULL, myResource);
		if (!myResourceData) throw std::runtime_error("Resource is empty?!");

		char* pMyBinaryData = (char*) ::LockResource(myResourceData);

		ofp.write(pMyBinaryData, myResourceSize);
		ofp.flush();

		UnlockResource(myResourceData);

		ofp.seek(0, ALLEGRO_SEEK_SET);
		return ofp;
	}

#endif

}