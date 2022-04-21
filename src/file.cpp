#include "../include/file.h"

namespace AllegroCPP {

	File::File(const std::string& path, const std::string& mode)
		: m_curr_path(path)
	{
		if (path.empty() || mode.empty()) throw std::invalid_argument("Path or mode is empty!");
		if (!al_is_system_installed()) al_init();

		if (!(m_fp = al_fopen(path.c_str(), mode.c_str()))) throw std::runtime_error("Could not open file!");
	}


	File::File(const std::string& path, const ALLEGRO_FILE_INTERFACE* interfac, const std::string& mode)
		: m_curr_path(path)
	{
		if (!interfac) throw std::invalid_argument("Interface is null!");
		if (path.empty() || mode.empty()) throw std::invalid_argument("Path or mode is empty!");
		if (!al_is_system_installed()) al_init();

		if (!(m_fp = al_fopen_interface(interfac, path.c_str(), mode.c_str()))) throw std::runtime_error("Could not open file!");
	}

	File::File(int fd, const std::string& mode)
	{
		if (fd < 0 || mode.empty()) throw std::invalid_argument("FD or mode is empty!");
		if (!al_is_system_installed()) al_init();		

		if (!(m_fp = al_fopen_fd(fd, mode.c_str()))) throw std::runtime_error("Could not open FD!");
	}

	File::File(File& fp, size_t slice_initial_size, const std::string& mode)
		: m_curr_path(fp.m_curr_path)
	{
		if (!fp.m_fp) throw std::invalid_argument("File is empty!");
		if (mode.empty()) throw std::invalid_argument("Mode is invalid!");
		if (!al_is_system_installed()) al_init();

		if (!(m_fp = al_fopen_slice(fp.m_fp, slice_initial_size, mode.c_str()))) throw std::runtime_error("Could not open file!");
	}


	File::~File()
	{
		if (m_fp) { al_fclose(m_fp); m_fp = nullptr; }
	}

	bool File::empty() const
	{
		return m_fp == nullptr;
	}

	bool File::valid() const
	{
		return m_fp != nullptr;
	}

	File::operator bool() const
	{
		return m_fp != nullptr;
	}

	File::operator ALLEGRO_FILE* ()
	{
		return m_fp;
	}

	File::File(File&& oth) noexcept
		: m_fp(oth.m_fp), m_curr_path(std::move(oth.m_curr_path))
	{
		oth.m_fp = nullptr;
	}

	void File::operator=(File&& oth) noexcept
	{
		if (m_fp) al_fclose(m_fp);
		m_curr_path = std::move(oth.m_curr_path);
		m_fp = oth.m_fp; 
		oth.m_fp = nullptr;
	}

	size_t File::read(void* dat, size_t len)
	{
		return m_fp ? al_fread(m_fp, dat, len) : 0;
	}

	size_t File::write(const void* dat, size_t len)
	{
		return m_fp ? al_fwrite(m_fp, dat, len) : 0;
	}

	bool File::flush()
	{
		return m_fp ? al_fflush(m_fp) : false;
	}

	int64_t File::tell() const
	{
		return m_fp ? al_ftell(m_fp) : -1;
	}

	bool File::seek(int64_t offset, int whence)
	{
		return m_fp ? al_fseek(m_fp, offset, whence) : false;
	}

	bool File::eof() const
	{
		return m_fp ? al_feof(m_fp) : true;
	}

	bool File::has_error() const
	{
		return m_fp ? (al_ferror(m_fp) != 0) : false;
	}

	file_error_report File::get_error() const
	{
		return m_fp ? file_error_report{ al_ferrmsg(m_fp), al_ferror(m_fp) } : file_error_report{"null", 0};
	}

	int File::ungetc(int c)
	{
		return m_fp ? al_fungetc(m_fp, c) : -1;
	}

	int64_t File::size() const
	{
		return m_fp ? al_fsize(m_fp) : -1;
	}

	int File::getc()
	{
		return m_fp ? al_fgetc(m_fp) : -1;
	}

	int File::putc(int c)
	{
		return m_fp ? al_fputc(m_fp, c) : -1;
	}

	int File::printformat(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		auto ret = al_vfprintf(m_fp, format, args);
		va_end(args);
		return ret;
	}

	int File::vprintformat(const char* format, va_list args)
	{
		return al_vfprintf(m_fp, format, args);
	}

	int16_t File::read16le()
	{
		return m_fp ? al_fread16le(m_fp) : -1;
	}

	int16_t File::read16be()
	{
		return m_fp ? al_fread16be(m_fp) : -1;
	}

	size_t File::write16le(int16_t c)
	{
		return m_fp ? al_fwrite16le(m_fp, c) : -1;
	}

	size_t File::write16be(int16_t c)
	{
		return m_fp ? al_fwrite16le(m_fp, c) : -1;
	}

	int32_t File::read32le()
	{
		return m_fp ? al_fread32le(m_fp) : -1;
	}

	int32_t File::read32be()
	{
		return m_fp ? al_fread32be(m_fp) : -1;
	}

	size_t File::write32le(int32_t c)
	{
		return m_fp ? al_fwrite32le(m_fp, c) : -1;
	}

	size_t File::write32be(int32_t c)
	{
		return m_fp ? al_fwrite32le(m_fp, c) : -1;
	}

	std::string File::gets(size_t max)
	{
		if (!m_fp) return {};
		std::string str(max, '\0');
		al_fgets(m_fp, str.data(), str.size());
		return str;
	}

	char* File::gets(char* const buf, size_t max)
	{
		return m_fp ? al_fgets(m_fp, buf, max) : nullptr;
	}

	std::shared_ptr<ALLEGRO_USTR> File::get_ustr()
	{
		if (!m_fp) return nullptr;
		return std::shared_ptr<ALLEGRO_USTR>(al_fget_ustr(m_fp), [](ALLEGRO_USTR* u) { al_ustr_free(u); });
	}

	bool File::puts(char const* str)
	{
		return m_fp ? (al_fputs(m_fp, str) >= 0) : false;
	}

	bool File::puts(const std::string& str)
	{
		return m_fp ? (al_fputs(m_fp, str.c_str()) >= 0) : false;
	}

	const std::string& File::get_filepath() const
	{
		return m_curr_path;
	}

	ALLEGRO_FILE* File::drop()
	{
		ALLEGRO_FILE* nf = m_fp;
		m_fp = nullptr;
		return nf;
	}

	File_tmp::File_tmp(const std::string& tmppath)
	{
		if (tmppath.empty()) throw std::invalid_argument("File format is empty!");
		if (!al_is_system_installed()) al_init();

		ALLEGRO_PATH* tmpptr = al_create_path(nullptr);
		if (!(m_fp = al_make_temp_file(tmppath.c_str(), &tmpptr))) {
			al_destroy_path(tmpptr);
			throw std::runtime_error("Could not open temp file!");
		}

		m_curr_path = al_path_cstr(tmpptr, ALLEGRO_NATIVE_PATH_SEP);
		al_destroy_path(tmpptr);
	}

	File_tmp::~File_tmp()
	{
		if (!m_curr_path.empty()) {
			std::remove(m_curr_path.c_str());
		}
	}

	File_tmp::File_tmp(File_tmp&& oth) noexcept 
		: File(std::move(oth))
	{
	}

	void File_tmp::operator=(File_tmp&& oth) noexcept
	{
		this->File::operator=(std::move(oth));
		if (!m_curr_path.empty()) {
			std::remove(m_curr_path.c_str());
		}
		m_curr_path = std::move(oth.m_curr_path);
	}

	File_memory::File_memory(size_t memlen)
	{
		if (memlen == 0) throw std::invalid_argument("Memory size can't be zero!");
		if (!al_is_system_installed()) al_init();

		if (!(m_mem = (char*)al_malloc(memlen))) throw std::runtime_error("Can't alloc!");
	}

	File_memory::~File_memory()
	{
		this->File::~File(); // close file FIRST
		if (m_mem) { al_free(m_mem); m_mem = nullptr; }
	}

	File_memory::File_memory(File_memory&& oth) noexcept
		: File(std::move(oth)), m_mem(oth.m_mem)
	{
		oth.m_mem = nullptr;
	}

	void File_memory::operator=(File_memory&& oth) noexcept
	{
		this->File::operator=(std::move(oth));
		if (m_mem) al_free(m_mem);
		m_mem = oth.m_mem;
		oth.m_mem = nullptr;
	}

	namespace _socketmap {

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

					sud->m_socks.push_back({ sock, *AI, type });
				}
				else {
					if (::connect(sock, AI->ai_addr, (int)AI->ai_addrlen) == SocketError) {
						closeSocket(sock);
						continue;
					}

					if (AI == nullptr) {
						sud->badflag |= static_cast<int32_t>(socket_errors::GETADDR_FAILED);
						freeaddrinfo(AddrInfo);
						return sud;
					}

					SocketAddrInfo cpy = *AI;
					freeaddrinfo(AddrInfo);

					sud->m_socks.push_back({ sock, cpy, type });
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

					oths.ptr->m_socks.push_back({ accep, trigginfo, socket_type::TCP_CLIENT });
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
						oths.ptr->m_socks.push_back({ ittrg->sock, trigginfo, socket_type::UDP_HOST_CLIENT });
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

				switch (curr.type) {
				case socket_type::TCP_CLIENT:
					res = ::recv(curr.sock, (char*)ptr, static_cast<int>(size), 0);
					if (res < 0) { sud->badflag |= static_cast<int32_t>(socket_errors::RECV_FAILED); }
					else if (res == 0) { sud->badflag |= static_cast<int32_t>(socket_errors::CLOSED); }
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
			return false; // invalid operation
		}

		int64_t sock_tell(ALLEGRO_FILE* fp)
		{
			return 0; // invalid operation
		}

		bool sock_seek(ALLEGRO_FILE* fp, int64_t offset, int whence)
		{
			return false; // invalid operation
		}

		bool sock_eof(ALLEGRO_FILE* fp)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			return (!sud || sud->m_socks.empty());
		}

		int sock_error(ALLEGRO_FILE* fp)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			return (!sud || sud->badflag != 0);
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

		socket_user_data::_eachsock::_eachsock(SocketType a, SocketAddrInfo b, socket_type c)
			: sock(a), info(b), type(c)
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

			m_fp = al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len);

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
			return !empty() && !has_error();
		}

		_FileSocket::operator bool() const
		{
			return !empty() && !has_error();
		}

	}

	File_client::File_client(_socketmap::socket_user_data* absorb)
	{
		set(absorb);
	}

	File_client::File_client(const std::string& addr, uint16_t port, int protocol, int family)
	{
		_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.addr = addr;
		conf.protocol = protocol;
		conf.family = family;
		conf.port = port;
		conf.host = false;

		m_fp = al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len);

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	File_client::File_client(const std::string& addr, uint16_t port, file_protocol protocol, file_family family)
	{
		_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.addr = addr;
		conf.protocol = static_cast<int>(protocol);
		conf.family = static_cast<int>(family);
		conf.port = port;
		conf.host = false;

		m_fp = al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len);

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

	bool File_client::set_timeout_read(unsigned long ms)
	{
		if (!m_fp) return false;
		_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp);
		if (!sod) return false;

		for (auto& i : sod->m_socks) {
			_socketmap::setsocktimeout_auto(i.sock, 1000);
		}
		return sod->m_socks.size() > 0;
	}

	File_host::File_host(uint16_t port, int protocol, int family)
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

		m_fp = al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len);

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	File_host::File_host(uint16_t port, file_protocol protocol, file_family family)
	{
		_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.protocol = static_cast<int>(protocol);
		conf.family = static_cast<int>(family);
		conf.port = port;
		conf.host = true;

		m_fp = al_fopen_interface(&_socketmap::socket_interface, (char*)&conf, (char*)&len);

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
		_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp);
		_socketmap::socket_user_data* sod_them = (_socketmap::socket_user_data*)al_get_file_userdata(oth.m_fp);
		if (!sod || !sod_them || !sod->has_host() || !sod_them->has_host()) return false;
		sod->m_socks.insert(sod->m_socks.end(), std::move_iterator(sod_them->m_socks.begin()), std::move_iterator(sod_them->m_socks.end()));
		sod_them->m_socks.clear();
		al_fclose(oth.m_fp);
		oth.m_fp = nullptr;
		return true;
	}

	File_client File_host::listen(long timeout)
	{
		if (!m_fp) throw std::runtime_error("Invalid state: null");
		_socketmap::socket_user_data* sod = (_socketmap::socket_user_data*)al_get_file_userdata(m_fp);
		if (!sod || !sod->has_host()) throw std::runtime_error("Invalid state: it's not a host?!");

		_socketmap::new_socket_user_data nsud;
		nsud.ptr = new _socketmap::socket_user_data;
		if (!nsud.ptr) throw std::bad_alloc();
		nsud.timeout = timeout;

		size_t confirm = al_fread(m_fp, (void*)&nsud, sizeof(_socketmap::new_socket_user_data));
		if (confirm != sizeof(_socketmap::socket_user_data)) {
			delete nsud.ptr;
			return File_client(nullptr);
		}

		return File_client(nsud.ptr);
	}


}