#pragma once

// remember font.h has to be able to eat the ALLEGRO_FILE to itself.

#ifndef ALLEGROCPP_DISABLE_FILESOCKET

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
using SocketType = SOCKET;
using SocketStorage = SOCKADDR_STORAGE;
using SocketAddrInfo = ADDRINFO;
using SocketSockAddrPtr = LPSOCKADDR;
using SocketPollFD = WSAPOLLFD;
constexpr SocketType SocketInvalid = INVALID_SOCKET;
constexpr int SocketError = SOCKET_ERROR;
constexpr int SocketTimeout = 0;
#define closeSocket(...) ::closesocket(__VA_ARGS__)
#define ioctlSocket(...) ::ioctlsocket(__VA_ARGS__)
#define pollSocket(...) ::WSAPoll(__VA_ARGS__)
#define theSocketError WSAGetLastError()
#define SocketBUFFERSMALL WSAEMSGSIZE
#define SocketWOULDBLOCK WSAEWOULDBLOCK
#define SocketNETRESET WSAENETRESET
#define SocketCONNRESET WSAECONNRESET
#define SocketPOLLIN POLLRDNORM
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
using SocketType = int;
using SocketStorage = sockaddr_storage;
using SocketAddrInfo = addrinfo;
using SocketSockAddrPtr = sockaddr*;
using SocketPollFD = pollfd;
constexpr SocketType SocketInvalid = -1;
constexpr int SocketError = -1;
constexpr int SocketTimeout = 0;
#define closeSocket(...) ::close(__VA_ARGS__)
#define ioctlSocket(...) ::ioctl(__VA_ARGS__)
#define pollSocket(...) ::poll(__VA_ARGS__)
#define theSocketError errno
#define SocketBUFFERSMALL EMSGSIZE
#define SocketWOULDBLOCK EWOULDBLOCK
#define SocketNETRESET ENETRESET
#define SocketCONNRESET ECONNRESET
#define SocketPOLLIN POLLIN
#endif
#define SocketGood(X) [&]{ const auto xg = (X); return (xg != SocketInvalid && xg != SocketError);}()

#endif // ALLEGROCPP_DISABLE_FILESOCKET

#include <allegro5/allegro.h>
#include <allegro5/allegro_memfile.h>

#include <string>
#include <stdexcept>
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>

namespace AllegroCPP {

	struct file_error_report {
		std::string msg;
		int err;
	};

	using File_shareable_ptr = std::shared_ptr<std::unique_ptr<ALLEGRO_FILE,std::function<void(ALLEGRO_FILE*)>>>;

	class File {
	protected:
		//ALLEGRO_FILE* m_fp = nullptr;
		File_shareable_ptr m_fp;
		std::string m_curr_path;
		File() = default;
	public:
		File(const std::string& path, const std::string& mode = "wb+");
		File(const std::string& path, const ALLEGRO_FILE_INTERFACE* interfac, const std::string& mode = "wb+");
		File(int fd, const std::string& mode = "wb+");
		File(File& fp, size_t slice_initial_size, const std::string& mode);
		virtual ~File();

		bool empty() const;
		bool valid() const;
		operator bool() const;
		operator ALLEGRO_FILE* ();
		operator File_shareable_ptr() const;

		File(const File&) = delete;
		File(File&&) noexcept;
		void operator=(const File&) = delete;
		void operator=(File&&) noexcept;

		size_t read(void*, size_t);
		size_t write(const void*, size_t);
		bool flush();
		int64_t tell() const;
		bool seek(int64_t offset, int whence);
		bool eof() const;
		bool has_error() const;
		file_error_report get_error() const;
		int ungetc(int);
		int64_t size() const;
		int getc();
		int putc(int);
		int printformat(const char* format, ...);
		int vprintformat(const char* format, va_list args);

		int16_t read16le();
		int16_t read16be();
		size_t write16le(int16_t);
		size_t write16be(int16_t);
		int32_t read32le();
		int32_t read32be();
		size_t write32le(int32_t);
		size_t write32be(int32_t);

		File& operator<<(const char* val);
		File& operator<<(const std::string& val);
		File& operator<<(const std::vector<char>& val);
		File& operator<<(bool val);
		File& operator<<(short val);
		File& operator<<(unsigned short val);
		File& operator<<(int val);
		File& operator<<(unsigned int val);
		File& operator<<(long val);
		File& operator<<(unsigned long val);
		File& operator<<(long long val);
		File& operator<<(unsigned long long val);
		File& operator<<(float val);
		File& operator<<(double val);
		File& operator<<(long double val);
		File& operator<<(std::streambuf* sb);
		File& operator<<(std::ostream& (*pf)(std::ostream&));
		File& operator<<(std::ios& (*pf)(std::ios&));
		File& operator<<(std::ios_base& (*pf)(std::ios_base&));

		File& operator>>(std::string& val);
		File& operator>>(std::vector<char>& val);
		File& operator>>(bool& val);
		File& operator>>(short& val);
		File& operator>>(unsigned short& val);
		File& operator>>(int& val);
		File& operator>>(unsigned int& val);
		File& operator>>(long& val);
		File& operator>>(unsigned long& val);
		File& operator>>(long long& val);
		File& operator>>(unsigned long long& val);
		File& operator>>(float& val);
		File& operator>>(double& val);
		File& operator>>(long double& val);

		virtual std::string gets(size_t);
		virtual char* gets(char* const buf, size_t max);
		std::shared_ptr<ALLEGRO_USTR> get_ustr(); // autofree
		virtual bool puts(char const* str);
		virtual bool puts(const std::string&);

		const std::string& get_filepath() const;

		// Reset internal ALLEGRO_FILE*. Valid only if not a memory file (else throw)
		//ALLEGRO_FILE* drop();
	};

	class File_tmp : public File {
		//using File::drop; // remove access
	public:
		File_tmp(const std::string& tmppath = "XXXXXXXX.tmp", const std::string& mode = "wb+");
		~File_tmp();

		File_tmp(const File_tmp&) = delete;
		File_tmp(File_tmp&&) noexcept;
		void operator=(const File_tmp&) = delete;
		void operator=(File_tmp&&) noexcept;
	};

	class File_memory : public File {
		char* m_mem = nullptr;

		//using File::drop; // remove access
	public:
		File_memory(size_t memlen);
		~File_memory();

		File_memory(const File_memory&) = delete;
		File_memory(File_memory&&) noexcept;
		void operator=(const File_memory&) = delete;
		void operator=(File_memory&&) noexcept;
	};

#ifndef ALLEGROCPP_DISABLE_FILESOCKET
	namespace _socketmap {

		enum class socket_errors : int32_t {
			SOCKET_INVALID				= 1 << 0,
			RECV_FAILED					= 1 << 1,
			SEND_FAILED					= 1 << 2,
			GETADDR_FAILED				= 1 << 3,
			ADDR_CANT_FIND				= 1 << 4,
			MODE_WAS_INVALID			= 1 << 5,
			HOST_PTR_RECV_FAIL			= 1 << 6,
			CLOSED						= 1 << 7
		};
		enum class socket_type : uint8_t {
			INVALID,
			TCP_HOST,
			TCP_CLIENT,
			UDP_HOST,
			UDP_HOST_CLIENT,
			UDP_CLIENT
		};

		struct socket_user_data {
			struct _eachsock {
				SocketType sock = SocketInvalid;
				SocketAddrInfo info{};
				socket_type type = socket_type::INVALID; // UDP_HOST_CLIENT is the only one that DOES NOT CLOSE SOCKET!
				_eachsock(SocketType, SocketAddrInfo, socket_type);
			};
			std::vector<_eachsock> m_socks; // SocketAddrInfo for UDP has last recv all the time.
			int32_t badflag = 0;
			bool has_host() const;
			void close_auto();
		};

		struct new_socket_user_data {
			socket_user_data* ptr = nullptr;
			long timeout = 0; // used on hosts only
		};

		struct socket_config {
			socket_user_data* prealloc = nullptr; // if set, that's it.
			std::string addr; // can be null for host or localhost
			int protocol;
			int family;
			uint16_t port = 0;
			bool host;
		};

		// socket_config* and &(sizeof(socket_config)) (as uint64_t)
		void* sock_open(const char* addr, const char* intptr);
		bool sock_close(ALLEGRO_FILE* fp);
		// tcp client and udp client return like recv. host expects void* to be socket_user_data and size == sizeof that (gets final value there)
		size_t sock_read(ALLEGRO_FILE* fp, void* ptr, size_t size);
		size_t sock_write(ALLEGRO_FILE* fp, const void* ptr, size_t size);
		bool sock_flush(ALLEGRO_FILE* fp);
		int64_t sock_tell(ALLEGRO_FILE* fp);
		bool sock_seek(ALLEGRO_FILE* fp, int64_t offset, int whence);
		bool sock_eof(ALLEGRO_FILE* fp);
		int sock_error(ALLEGRO_FILE* fp);
		const char* sock_errmsg(ALLEGRO_FILE* fp);
		void sock_clearerr(ALLEGRO_FILE* fp);
		int sock_ungetc(ALLEGRO_FILE* fp, int c);
		off_t sock_size(ALLEGRO_FILE*);

		std::vector<socket_user_data::_eachsock>::const_iterator sock_listen(const std::vector<socket_user_data::_eachsock>& servers, const long timeout);
		SocketType sock_listen(const std::vector<SocketType>& servers, const long timeout);
		void setsocktimeout_auto(SocketType, unsigned long ms);
		
		static ALLEGRO_FILE_INTERFACE socket_interface =
		{
		   sock_open,
		   sock_close,
		   sock_read,
		   sock_write,
		   sock_flush,
		   sock_tell,
		   sock_seek,
		   sock_eof,
		   sock_error,
		   sock_errmsg,
		   sock_clearerr,
		   sock_ungetc,
		   sock_size
		};

		class _FileSocket : protected File {
#ifdef _WIN32 
			struct _winsock_start { WSADATA wsaData = WSADATA(); _winsock_start(); ~_winsock_start(); };
			static _winsock_start __winsock;
#endif
		protected:
			// used in host type, when host listen() or something and them boom, child.
			void set(_socketmap::socket_user_data* absorb);

			_FileSocket();
		public:
			_FileSocket(_FileSocket&&) noexcept;
			void operator=(_FileSocket&&) noexcept;
			_FileSocket(const _FileSocket&) = delete;
			void operator=(const _FileSocket&) = delete;

			bool empty() const;
			bool valid() const;
			operator bool() const;

			using File::operator=;

			using File::operator ALLEGRO_FILE*;

			using File::read;
			using File::write;
			using File::eof;
			using File::has_error;
			using File::get_error;
			using File::getc;
			using File::putc;
			using File::printformat;
			using File::vprintformat;

			using File::operator<<;
			using File::operator>>;

			std::string gets(size_t);
			char* gets(char* const buf, size_t max);

			bool puts(char const* str);
			bool puts(const std::string&);
		};
	}

	class File_host;

	enum class file_protocol{TCP = SOCK_STREAM, UDP = SOCK_DGRAM};
	enum class file_family{ANY = PF_UNSPEC, IPV4 = PF_INET, IPV6 = PF_INET6};

	class File_client : public _socketmap::_FileSocket {
		friend class File_host; // so host can gen File_client

		File_client(_socketmap::socket_user_data* absorb);
	public:
		File_client(const std::string& addr, uint16_t port, int protocol, int family = PF_UNSPEC);
		File_client(const std::string& addr, uint16_t port, file_protocol protocol = file_protocol::TCP, file_family family = file_family::ANY);

		File_client(const File_client&) = delete;
		File_client(File_client&&) noexcept;
		void operator=(const File_client&) = delete;
		void operator=(File_client&&) noexcept;

		bool set_timeout_read(unsigned long ms);
	};

	class File_host : public _socketmap::_FileSocket {
		using _FileSocket::write; // hide
		using _FileSocket::read; // hide
	public:
		File_host(uint16_t port, int protocol, int family = PF_UNSPEC);
		File_host(uint16_t port, file_protocol protocol = file_protocol::TCP, file_family family = file_family::ANY);

		File_host(const File_host&) = delete;
		File_host(File_host&&) noexcept;
		void operator=(const File_host&) = delete;
		void operator=(File_host&&) noexcept;

		bool combine(File_host&&);

		File_client listen(long timeout = 500);
	};

#endif // ALLEGROCPP_DISABLE_FILESOCKET
}