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
		return m_fp ? file_error_report{ al_ferrmsg(m_fp), al_ferror(m_fp) } : file_error_report{"", 0};
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

	FileTmp::FileTmp(const std::string& tmppath)
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

	FileTmp::~FileTmp()
	{
		if (!m_curr_path.empty()) {
			std::remove(m_curr_path.c_str());
		}
	}

	FileTmp::FileTmp(FileTmp&& oth) noexcept 
		: File(std::move(oth))
	{
	}

	void FileTmp::operator=(FileTmp&& oth) noexcept
	{
		this->File::operator=(std::move(oth));
		if (!m_curr_path.empty()) {
			std::remove(m_curr_path.c_str());
		}
		m_curr_path = std::move(oth.m_curr_path);
	}

	FileMem::FileMem(size_t memlen)
	{
		if (memlen == 0) throw std::invalid_argument("Memory size can't be zero!");
		if (!al_is_system_installed()) al_init();

		if (!(m_mem = (char*)al_malloc(memlen))) throw std::runtime_error("Can't alloc!");
	}

	FileMem::~FileMem()
	{
		this->File::~File(); // close file FIRST
		if (m_mem) { al_free(m_mem); m_mem = nullptr; }
	}

	FileMem::FileMem(FileMem&& oth) noexcept
		: File(std::move(oth)), m_mem(oth.m_mem)
	{
		oth.m_mem = nullptr;
	}

	void FileMem::operator=(FileMem&& oth) noexcept
	{
		this->File::operator=(std::move(oth));
		if (m_mem) al_free(m_mem);
		m_mem = oth.m_mem;
		oth.m_mem = nullptr;
	}

	namespace AllegroCPP_socketmap {

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
			sud->is_host = theconf.host;
			sud->is_udp_shared = false; // never here
			sud->lastprot = theconf.protocol;

			SocketAddrInfo* AddrInfo = nullptr;
			SocketAddrInfo Hints{};

			Hints.ai_socktype = theconf.protocol;
			Hints.ai_family = theconf.family;
			if (sud->is_host) Hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;

			char Port[8]{};
#ifdef _WIN32
			sprintf_s(Port, "%hu", theconf.port);
			ZeroMemory(&Hints, sizeof(Hints));
#else
			sprintf(Port, "%hu", theconf.port);
			memset(&Hints, 0, sizeof(Hints));
#endif
			if (getaddrinfo(theconf.addr.size() ? theconf.addr.c_str() : nullptr, Port, &Hints, &AddrInfo) != 0) {
				sud->badflag |= static_cast<int32_t>(socket_errors::GETADDR_FAILED);
				return sud;
			}

			int i = 0;
			for (SocketAddrInfo* AI = AddrInfo; AI != nullptr && i != FD_SETSIZE; AI = AI->ai_next)
			{
				if (sud->is_host && (AI->ai_family != PF_INET) && (AI->ai_family != PF_INET6)) continue;

				SocketType sock = SocketInvalid;

				if ((sock = ::socket(AI->ai_family, AI->ai_socktype, AI->ai_protocol)) == SocketInvalid) continue;

				if (sud->is_host) {
					int on = 1;
					setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on));

					if (::bind(sock, AI->ai_addr, (int)AI->ai_addrlen) == SocketError) {
						closeSocket(sock);
						continue;
					}

					// Specific for TCP
					if (AI->ai_protocol == SOCK_STREAM) {
						if (::listen(sock, 5) == SocketError) {
							closeSocket(sock);
							continue;
						}
					}

					sud->m_socks.push_back(sock);
				}
				else {
					if (::connect(sock, AI->ai_addr, (int)AI->ai_addrlen) == SocketError) {
						closeSocket(sock);
						continue;
					}

					freeaddrinfo(AddrInfo);

					if (AI == nullptr) {
						sud->badflag |= static_cast<int32_t>(socket_errors::GETADDR_FAILED);
						return sud;
					}

					sud->m_socks.push_back(sock);
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
			for(auto& i : sud->m_socks) closeSocket(i);
			delete sud;
			return true;
		}

		size_t sock_read(ALLEGRO_FILE* fp, void* ptr, size_t size)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			if (!sud) return 0;
			if (sud->is_host || sud->m_socks.size() == 0) { sud->badflag |= static_cast<int32_t>(socket_errors::MODE_WAS_INVALID); return 0; }
			auto& sok = sud->m_socks[0];
			if (sok == SocketInvalid) { sud->badflag |= static_cast<int32_t>(socket_errors::SOCKET_INVALID); return 0; }
			int res = ::recv(sok, (char*)ptr, static_cast<int>(size), 0);
			if (res < 0) { sud->badflag |= static_cast<int32_t>(socket_errors::RECV_FAILED); }
			return res > 0 ? res : 0;
		}

		size_t sock_write(ALLEGRO_FILE* fp, const void* ptr, size_t size)
		{
			socket_user_data* sud = (socket_user_data*)al_get_file_userdata(fp);
			if (!sud) return 0;
			if (sud->is_host || sud->m_socks.size() == 0) { sud->badflag |= static_cast<int32_t>(socket_errors::MODE_WAS_INVALID); return 0; }
			auto& sok = sud->m_socks[0];
			if (sok == SocketInvalid) { sud->badflag |= static_cast<int32_t>(socket_errors::SOCKET_INVALID); return 0; }
			int res = ::send(sok, (const char*)ptr, static_cast<int>(size), 0);
			if (res < 0) { sud->badflag |= static_cast<int32_t>(socket_errors::SEND_FAILED); }
			return res > 0 ? res : 0;
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

			if (sud->badflag & static_cast<int32_t>(socket_errors::SOCKET_INVALID))	  return "Socket is in a invalid state";
			if (sud->badflag & static_cast<int32_t>(socket_errors::RECV_FAILED))	  return "Socket failed to recv at least once (-1)";
			if (sud->badflag & static_cast<int32_t>(socket_errors::SEND_FAILED))	  return "Socket failed to send at least once (-1)";
			if (sud->badflag & static_cast<int32_t>(socket_errors::GETADDR_FAILED))	  return "Getaddrinfo failed to find a valid option";
			if (sud->badflag & static_cast<int32_t>(socket_errors::ADDR_CANT_FIND))	  return "Could not find a valid AddrInfo";
			if (sud->badflag & static_cast<int32_t>(socket_errors::MODE_WAS_INVALID)) return "There was no socket set";
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

		SocketType sock_listen(std::vector<SocketType>& servers, const long timeout)
		{
			if (servers.size() == 0) return SocketInvalid;

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

	}

#ifdef _WIN32 
	FileSocket::_winsock_start FileSocket::__winsock;

	FileSocket::_winsock_start::_winsock_start()
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) throw std::runtime_error("Can't start WSA");
	}

	FileSocket::_winsock_start::~_winsock_start()
	{
		WSACleanup();
	}
#endif

	FileSocket::FileSocket(AllegroCPP_socketmap::socket_user_data* absorb)
	{
		if (!absorb) return; // invalid, got null, like empty file or closed.

		AllegroCPP_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = absorb;

		m_fp = al_fopen_interface(&AllegroCPP_socketmap::socket_interface, (char*)&conf, (char*)&len);

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	bool FileSocket::_listen(long timeout, SocketType* sok, AllegroCPP_socketmap::socket_user_data** sodnd) const
	{
		if (!m_fp) return false;
		AllegroCPP_socketmap::socket_user_data* sod = (AllegroCPP_socketmap::socket_user_data*)al_get_file_userdata(m_fp);
		if (!sod || sod->m_socks.empty()) return false;
		if (sodnd) *sodnd = sod; // copy
		return (sok ? (*sok = AllegroCPP_socketmap::sock_listen(sod->m_socks, timeout)) : AllegroCPP_socketmap::sock_listen(sod->m_socks, timeout)) != SocketInvalid;
	}

	FileSocket::FileSocket(const std::string& addr, uint16_t port, int protocol, int family)
	{
		AllegroCPP_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.addr = addr;
		conf.protocol = protocol;
		conf.family = family;
		conf.port = port;
		conf.host = false;

		m_fp = al_fopen_interface(&AllegroCPP_socketmap::socket_interface, (char*)&conf, (char*)&len);

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	FileSocket::FileSocket(uint16_t port, int protocol, int family)
	{
		AllegroCPP_socketmap::socket_config conf;
		uint64_t len = sizeof(conf);

		conf.prealloc = nullptr;
		conf.protocol = protocol;
		conf.family = family;
		conf.port = port;
		conf.host = true;

		m_fp = al_fopen_interface(&AllegroCPP_socketmap::socket_interface, (char*)&conf, (char*)&len);

		if (!m_fp) throw std::runtime_error("Could not create FileSocket");
	}

	bool FileSocket::combine(FileSocket&& oth)
	{
		if (!m_fp || !oth.m_fp) return false;
		AllegroCPP_socketmap::socket_user_data* sod = (AllegroCPP_socketmap::socket_user_data*)al_get_file_userdata(m_fp);
		AllegroCPP_socketmap::socket_user_data* sod_them = (AllegroCPP_socketmap::socket_user_data*)al_get_file_userdata(oth.m_fp);
		if (!sod || !sod_them) return false;
		if (!sod->is_host || !sod_them->is_host) return false;

		sod->m_socks.insert(sod->m_socks.end(), std::move_iterator(sod_them->m_socks.begin()), std::move_iterator(sod_them->m_socks.end()));
		sod_them->m_socks.clear();
		al_fclose(oth.m_fp);
		oth.m_fp = nullptr;
		return true;

	}

	bool FileSocket::has_listen() const
	{
		return _listen(1);
	}

	FileSocket FileSocket::listen(long timeout) const
	{
		SocketType sock = SocketInvalid;
		AllegroCPP_socketmap::socket_user_data* sod = nullptr;

		if (!_listen(timeout, &sock, &sod)) return FileSocket(nullptr);

		SocketStorage From{};
		socklen_t FromLen = sizeof(SocketStorage);

		AllegroCPP_socketmap::socket_user_data* new_sod = new AllegroCPP_socketmap::socket_user_data();
		new_sod->is_udp_shared = (sod->lastprot & SOCK_DGRAM);
		new_sod->is_host = false;
		new_sod->badflag = 0;
		new_sod->lastprot = sod->lastprot;
		new_sod->m_socks.push_back((sod->lastprot & SOCK_DGRAM) ? sock : accept(sock, (SocketSockAddrPtr)&From, &FromLen));
		if (new_sod->m_socks[0] == SocketInvalid) { // pain
			delete new_sod;
			return FileSocket(nullptr);
		}
		return FileSocket(new_sod);
	}

}